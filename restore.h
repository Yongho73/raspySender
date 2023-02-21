#pragma once
#include <dirent.h>

int renameErrorToReady(); /* error 파일 ready 파일로 변경 */
int readReady();          /* 실패건 재전송 완료후 파일명 변경 */