// ReSharper disable CppClangTidyCertErr33C
#include "LaunchOrange.h"
#include <Windows.h>
#include <cstdio>
#include <process.h>
#include <ctime>
#include <string>

static wchar_t pyexe[2048] = L"python.exe";

//
void SetOrangePython(const wchar_t* path)
{
	if (!path || *path == L'\0')
		return;
	wcscpy_s(pyexe, path);
}

//
static std::wstring get_temp_filename()
{
	// 템프
	wchar_t* temp = nullptr;
	std::size_t sz = 0;
	if (_wdupenv_s(&temp, &sz, L"TEMP") != 0)
	{
		// 실패하면 C: 루트
		temp = _wcsdup(L"C:");
	}

	// 타임스탬프
	time_t tt;
	time(&tt);
	tm ti = {};
	localtime_s(&ti, &tt);
	wchar_t ts[64];
	wcsftime(ts, 64 - 1, L"%G%m%d%H%M%S", &ti);

	//
	std::wstring r(temp);
	r += L"\\z";
	r += ts;

	// 정리하고 반환
	free(temp);
	return r;
}

// 런치 오렌지
int LaunchOrange(const wchar_t* model, const wchar_t* data, char** csv)
{
	if (!model || *model == L'\0')
		return 1;
	if (!data || *data == L'\0')
		return 1;
	if (!csv)
		return 1;

	//
	const auto fhdr = get_temp_filename();

	const auto pypat =
		"import Orange\n" \
		"import pickle\n" \
		"import pandas as pd\n" \
		"\n" \
		"model = pickle.load(open('%ls', 'rb'))\n" \
		"test = Orange.data.Table('%ls')\n" \
		"pred_ind = model(test)\n" \
		"Pred = pd.DataFrame(model.domain.class_var.str_val(i) for i in pred_ind)\n" \
		"Pred_list = [model.domain.class_var.str_val(i) for i in pred_ind]\n" \
		"prob = pd.DataFrame(model(test, model.Probs))\n" \
		"prob['prediction'] = Pred\n" \
		"prob.to_csv('%ls.csv')\n";

	//
	wchar_t pyfile[2048];
	wcscpy_s(pyfile, fhdr.c_str());
	wcscat_s(pyfile, L".py");

	FILE* fp;
	_wfopen_s(&fp, pyfile, L"wt");
	fprintf_s(fp, pypat, model, data, fhdr.c_str());
	fclose(fp);

	//
	wchar_t cmdline[2048];
	wcscpy_s(cmdline, pyexe);
	wcscat_s(cmdline, L" \"");
	wcscat_s(cmdline, pyfile);
	wcscat_s(cmdline, L"\"");

	STARTUPINFO si = { sizeof(STARTUPINFO), };
	PROCESS_INFORMATION pi = { nullptr, };

	const auto ret = CreateProcess(
		nullptr,
		cmdline,
		nullptr,
		nullptr,
		FALSE,
		0,
		nullptr,
		nullptr,
		&si, &pi);

	if (!ret)
	{
		_wunlink(pyfile);
		return 3;
	}

	CloseHandle(pi.hThread);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	_wunlink(pyfile);

	//
	wchar_t csvfile[2048];
	wcscpy_s(csvfile, fhdr.c_str());
	wcscat_s(csvfile, L".csv");

	if (_wfopen_s(&fp, csvfile, L"rt") != 0)
	{
		// 파일이 없으면 수행 실패로
		return 2;
	}

	std::string read;
	char rbuf[1025];
	for (;;)
	{
		const auto sz = fread(rbuf, 1, 1024, fp);
		if (sz > 0)
		{
			rbuf[sz] = '\0';
			read += rbuf;
		}

		if (feof(fp))
			break;
	}

	fclose(fp);

	const auto pp = static_cast<char*>(malloc(read.length() + 5));
	memcpy(pp, read.c_str(), read.length());
	pp[read.length()] = '\0';
	*csv = pp;

	_wunlink(csvfile);

	// 끝
	return 0;
}
