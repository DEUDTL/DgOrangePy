#pragma once

/**
 * \brief 파이선 경로를 지정합니다. 안하면 기본 "python.exe"
 * \param path 파이선 경로
 */
extern "C" void SetOrangePython(const wchar_t* path);

/**
 * \brief 오렌지AI를 실행
 * \param model 모델 파일 이름
 * \param data 데이터 파일 이름
 * \param csv 반환 csv. 반드시 free()로 해제하세요
 */
extern "C" int LaunchOrange(const wchar_t* model, const wchar_t* data, char** csv);
