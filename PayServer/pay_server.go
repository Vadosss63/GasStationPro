package main

import (
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"strconv"
	"sync"
)

var Token string
var User string
var Password string

var port io.ReadWriteCloser
var isOpenPort bool
var sendSumToWashCom []byte
var mux sync.Mutex

const (
	protocol = "unix"
	sockAddr = "/tmp/t_cash.sock"
)

func init() {
	Token = "45806cb39b319a29bdf6c677d4d00e9c0d3b3c7a"
	User = "t_box"
	Password = "t_box"
	fmt.Println("init")
}

type PostJson struct {
	Number string `json:"number"` //:0-N
	Status string `json:"status"` //:("free"|"wrong"|"busy")
}
type AllPostJson struct {
	Status string     `json:"status"`
	Posts  []PostJson `json:"posts"`
}

type StatusSend struct {
	Status string `json:"status"`
}

type StatusStatSend struct {
	Status    string `json:"status"`
	Statistic string `json:"statistic"`
}

type AnswerToken struct {
	Access_token string  `json:"access_token"`
	Expires_in   int     `json:"expires_in"`
	Token_type   string  `json:"token_type"`
	Scope        *string `json:"scope"`
}

type ErrorAnswerToken struct {
	Error             string `json:"error"`             //"invalid_client"
	Error_description string `json:"error_description"` //"client credentials are required"
}

type RequestWash struct {
	postNumber int
	sum        int
}

func getIntVal(val string) (int, bool) {
	sum, err := strconv.Atoi(val)
	if err != nil {
		fmt.Println(err)
		return 0, false
	}
	return sum, true
}

func getWash(r *http.Request) (RequestWash, bool) {

	postNumber, ok := getIntVal(r.FormValue("postNumber"))
	if ok == true {
		requestWash := RequestWash{}
		requestWash.postNumber = postNumber
		sum, ok := getIntVal(r.FormValue("sum"))
		if ok == true {
			requestWash.sum = sum
			fmt.Println(requestWash)
			return requestWash, true
		}
	}
	return RequestWash{}, false
}

func getPosts() AllPostJson {
	allPostJson := AllPostJson{
		Posts: []PostJson{{Number: "1", Status: "free"},
			{Number: "2", Status: "free"},
			{Number: "3", Status: "free"},
			{Number: "4", Status: "free"},
			{Number: "5", Status: "free"},
			{Number: "6", Status: "free"},
			{Number: "7", Status: "free"},
			{Number: "8", Status: "free"},
			{Number: "9", Status: "free"},
			{Number: "10", Status: "free"},
			{Number: "11", Status: "free"},
			{Number: "12", Status: "free"}},
		Status: "ready"}
	return allPostJson
}

func handleRequest(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	statusSending := StatusSend{Status: "wrong"}
	// key := r.FormValue("apikey")
	for k, v := range r.Header {
		fmt.Println("Header field ", k, "Value ", v)
	}

	// if key == Token {
	action := r.FormValue("action")
	//session := r.FormValue("session")
	// fmt.Println(key)
	//fmt.Println(session)

	switch action {
	case "0": // Получение готовности поста
		postNumber, ok := getIntVal(r.FormValue("postNumber"))
		if ok {
			statusSending.Status = "free"
			w.WriteHeader(http.StatusOK)
			json.NewEncoder(w).Encode(statusSending)
			fmt.Println("Get post", postNumber)
		}
		return
	case "1": // Получение списка постов
		allPostJson := getPosts()
		w.WriteHeader(http.StatusOK)
		json.NewEncoder(w).Encode(allPostJson)
		fmt.Println("Get posts")
		return
	case "2": // Проверка готовности мойки
		_, ok := getWash(r)
		if ok {
			statusSending.Status = "successful"
			fmt.Println("Check posts")
		}
		break
	case "3": // Старт мойки
		req, ok := getWash(r)
		if ok {
			if req.postNumber > 12 {
				fmt.Println("Error postNumber: ", req.postNumber)
				break
			}
			fmt.Println("Start wash")
			if sendSum(req.postNumber, req.sum) {
				fmt.Println("Run wash")
				statusSending.Status = "successful"
			}
		}
		break
	default:
		w.WriteHeader(http.StatusNoContent)
		json.NewEncoder(w).Encode(statusSending)
		return
	}
	// }
	w.WriteHeader(http.StatusOK)
	json.NewEncoder(w).Encode(statusSending)
}

func handleStatisticRequest(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")

	statusStatSend := StatusStatSend{Status: "wrong", Statistic: "empty"}

	// http://127.0.0.1:8081/api/statistic?user=t_cash&password=cash
	user := r.FormValue("user")
	password := r.FormValue("password")

	if user == "t_cash" && password == "cash" {
		stat, ok := getStat()
		if ok {
			statusStatSend = StatusStatSend{Status: "successful", Statistic: stat}
		}
	}

	w.WriteHeader(http.StatusOK)
	json.NewEncoder(w).Encode(statusStatSend)
}

func getToken(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	user, password, _ := r.BasicAuth()
	fmt.Println(user, password)

	if user != User || password != Password {
		errorAnswerToken := ErrorAnswerToken{
			Error:             "invalid_client",
			Error_description: "client credentials are required"}
		w.WriteHeader(http.StatusOK)
		json.NewEncoder(w).Encode(errorAnswerToken)
		fmt.Println("Error u:p", user, password)
		return
	}

	fmt.Println("Get Token")
	answerToken := AnswerToken{Access_token: Token, Expires_in: 3600, Token_type: "bearer", Scope: nil}
	w.WriteHeader(http.StatusOK)
	json.NewEncoder(w).Encode(answerToken)
}

func getStat() (string, bool) {
	mux.Lock()
	defer mux.Unlock()

	conn, err := net.Dial(protocol, sockAddr)
	//defer conn.Close()

	if err != nil {
		log.Fatal(err)
		return "", false
	}

	msg := fmt.Sprintf("cmd:%d,post:%d,sum:%d", 1, 0, 0)

	_, err = conn.Write([]byte(msg))
	if err != nil {
		log.Fatal(err)
		return "", false
	}

	err = conn.(*net.UnixConn).CloseWrite()
	if err != nil {
		log.Fatal(err)
		return "", false
	}

	b, err := ioutil.ReadAll(conn)
	if err != nil {
		log.Fatal(err)
		return "", false
	}
	return string(b), true
}

func sendSum(post int, sum int) bool {
	mux.Lock()
	defer mux.Unlock()

	conn, err := net.Dial(protocol, sockAddr)
	//defer conn.Close()

	if err != nil {
		log.Fatal(err)
		return false
	}

	msg := fmt.Sprintf("cmd:%d,post:%d,sum:%d", 0, post, sum)
	_, err = conn.Write([]byte(msg))
	if err != nil {
		log.Fatal(err)
		return false
	}

	err = conn.(*net.UnixConn).CloseWrite()
	if err != nil {
		log.Fatal(err)
		return false
	}

	b, err := ioutil.ReadAll(conn)
	if err != nil {
		log.Fatal(err)
		return false
	}

	answer := string(b)
	fmt.Println(answer)

	if answer == "ok" {
		return true
	}
	return false
}

func main() {

	http.HandleFunc("/api/wash", func(w http.ResponseWriter, r *http.Request) {
		handleRequest(w, r)
	})

	http.HandleFunc("/api/token", func(w http.ResponseWriter, r *http.Request) {
		getToken(w, r)
	})

	http.HandleFunc("/api/statistic", func(w http.ResponseWriter, r *http.Request) {
		handleStatisticRequest(w, r)
	})

	http.ListenAndServe(":8081", nil)
	//http.ListenAndServeTLS(":8080", "cert.pem", "key.pem", nil)
}
