# Create virtual ports
socat -d -d pty,raw,link=/tmp/ttyV1 pty,raw,link=/tmp/ttyV2

# Run simulator

python simulator.py /tmp/ttyV2