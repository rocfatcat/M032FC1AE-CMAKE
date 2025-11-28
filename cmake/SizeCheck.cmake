# size_check.cmake 內容

# 獲取檔案大小並存入 SIZE_VAR 變數
file(SIZE ${FILE_PATH} SIZE_VAR)

# 打印檔案名稱和大小
message(STATUS "${FILE_NAME} size: ${SIZE_VAR} Bytes")