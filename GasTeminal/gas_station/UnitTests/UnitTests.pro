TEMPLATE = subdirs

LIB_PATH = $$files(ut_*)
for(DIR, LIB_PATH) {
    exists($$DIR) {
        SUBDIRS += $$DIR
    }
}
