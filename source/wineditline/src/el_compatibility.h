#ifndef _EDITLINE_COMPATIBILITY_H_
#define _EDITLINE_COMPATIBILITY_H_

#ifndef _MSC_VER

#  define wcsncpy_s(DST, DST_LEN, SRC, SRC_LEN)   wcsncpy((DST), (SRC), (SRC_LEN))
#  define wcscpy_s(DST, DST_LEN, SRC)             wcscpy((DST), (SRC))
#  define strcpy_s(DST, DST_LEN, SRC)             strcpy((DST), (SRC))
#  define wcscat_s(DST, DST_LEN, SRC)             wcscat((DST), (SRC))
#  define swprintf_s(DST, DST_LEN, ...)           swprintf((DST), __VA_ARGS__)
#  define swscanf_s                               swscanf
#  define sscanf_s                                sscanf

static int _wgetenv_s( 
  size_t *pReturnValue,
  wchar_t *buffer,
  size_t numberOfElements,
  const wchar_t *varname 
) {
  wchar_t *val;
  size_t len;
  
  *pReturnValue = 0;
  
  val = _wgetenv(varname);
  if(!val)
    return -1;
    
  len = wcslen(val);
  *pReturnValue = len + 1;
  if(len + 1 > numberOfElements || !buffer)
    return -1;
  
  wcscpy(buffer, val);
  return 0;
}

static int _wfopen_s(
  FILE** pFile,
  const wchar_t *filename,
  const wchar_t *mode 
) {
  if(!pFile)
    return -1;
   
  *pFile = _wfopen(filename, mode);
  if(*pFile)
    return 0;
  
  return -1;
}

#endif

#endif
