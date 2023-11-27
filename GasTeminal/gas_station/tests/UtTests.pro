TEMPLATE = subdirs

LIB_PATH = $$files(ut_*)
for(DIR_BLOCKS ,LIB_PATH){
   exists($$DIR_BLOCKS){
         SUBDIRS += $$DIR_BLOCKS
}
}
