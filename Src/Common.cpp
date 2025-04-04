#include "Common.h"

std::wstring ConvertToWString(const char* str)
{
	// ACP949 -> �����ڵ�� ��ȯ (���̸� Ȯ��)
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
	if (len == 0) return L"";

	// ACP949 -> �����ڵ�� ��ȯ (result�� ����)
	std::wstring result(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, str, -1, &result[0], len);

	// ���� wstring�� '\0'�� �־��µ� ���� �����Ҷ� '\0'�� �� �����Ƿ� 1�� ����
	result.pop_back();
	return result;
}
