#include <iostream>
#include "LaunchOrange.h"

int main()
{
	std::cout << "Hello Orange!\n";

	//SetOrangePython(L"C:\\Program Files\\WindowsApps\\PythonSoftwareFoundation.Python.3.10_3.10.2544.0_x64__qbz5n2kfra8p0\\python3.10.exe");

	char* csv = nullptr;
	const auto ret = LaunchOrange(
		L"F:\\WORKAF\\DEU\\OrangePy\\Modelexport.pkcls",	// 모델
		L"F:\\WORKAF\\DEU\\OrangePy\\Modeltest.tab",		// 데이터
		&csv);
	if (ret != 0)
		std::cerr << "파이선 실행에 문제가 있어요: " << ret;

	if (csv)
	{
		// csv 데이터 조작
		free(csv);
	}

	return 0;
}

