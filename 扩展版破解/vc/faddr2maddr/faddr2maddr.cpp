#include <stdio.h>
/*
Purpose:PE�ļ����ڴ�ƫ�����ļ�ƫ���໥ת��,������ϵͳΪ�������ƫ��ת��
szFileName:�ļ���
dwAddr:��Ҫת����ƫ��ֵ
bFile2RVA:�Ƿ����ļ�ƫ�Ƶ��ڴ�ƫ�Ƶ�ת����1 - dwAddr��������ļ�ƫ�ƣ��˺��������ڴ�ƫ��
    0 - dwAddr��������ڴ�ƫ�ƣ��˺��������ļ�ƫ��
����ֵ�����Ӧ��ƫ��ֵ,ʧ�ܷ���-1
*/

DWORD AddressConvert(char szFileName[], DWORD dwAddr, BOOL bFile2RVA)
{
 char *lpBase = NULL;
 DWORD dwRet = -1;
 //1.���Ƚ��ļ������ڴ�
 if(szFileName[0] == 0)
 {
  return -1;
 }

 FILE *fp = fopen(szFileName, "rb");
 if(fp == 0)
 {
  return -1;
 }

 fseek(fp, 0, SEEK_END);
 DWORD dwFileSize = ftell(fp);
 if(dwFileSize == 0)
 {
  return -1;
 }

 lpBase = new char[dwFileSize];
 memset(lpBase, 0, dwFileSize);
 fseek(fp, 0, SEEK_SET);
 fread(lpBase, 1, dwFileSize, fp);
 fclose(fp);

 //2.��ȡ���ļ�����Ϣ���ļ��ڴ���뷽ʽ�Լ��������������������ָ��ָ��������һ������ͷ��
 PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpBase;
 PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((unsigned long)lpBase + pDosHeader->e_lfanew);
 
 DWORD dwMemAlign = pNtHeader->OptionalHeader.SectionAlignment;
 DWORD dwFileAlign = pNtHeader->OptionalHeader.FileAlignment;
 int dwSecNum = pNtHeader->FileHeader.NumberOfSections;
 PIMAGE_SECTION_HEADER pSecHeader = (PIMAGE_SECTION_HEADER)((char *)lpBase + pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS));
 DWORD dwHeaderSize = 0;
 
 if(!bFile2RVA)  // �ڴ�ƫ��ת��Ϊ�ļ�ƫ��
 { 
  //����Ҫת�Ƶ�ƫ���Ƿ���PEͷ�ڣ������������ƫ����ͬ
  dwHeaderSize = pNtHeader->OptionalHeader.SizeOfHeaders;
  if(dwAddr <= dwHeaderSize)
  {
   delete lpBase;
   lpBase = NULL;
   return dwAddr;
  }
  else //����PEͷ��鿴�õ�ַ���ĸ�������
  {
   for(int i = 0; i < dwSecNum; i++)
   {
    DWORD dwSecSize = pSecHeader[i].Misc.VirtualSize;
    if((dwAddr >= pSecHeader[i].VirtualAddress) && (dwAddr <= pSecHeader[i].VirtualAddress + dwSecSize))
    {
     //3.�ҵ��ø�ƫ�ƣ����ļ�ƫ�� = ��������ļ�ƫ�� + ����ƫ�� - ��������ڴ�ƫ�ƣ�
     dwRet = pSecHeader[i].PointerToRawData + dwAddr - pSecHeader[i].VirtualAddress;
    }
   }
  }
 }
 else // �ļ�ƫ��ת��Ϊ�ڴ�ƫ��
 {
  dwHeaderSize = pNtHeader->OptionalHeader.SizeOfHeaders;
  //����Ҫת�Ƶ�ƫ���Ƿ���PEͷ�ڣ������������ƫ����ͬ
  if(dwAddr <= dwHeaderSize)
  {
   delete lpBase;
   lpBase = NULL;
   return dwAddr;
  }
  else//����PEͷ��鿴�õ�ַ���ĸ�������
  {
   for(int i = 0; i < dwSecNum; i++)
   {
    DWORD dwSecSize = pSecHeader[i].Misc.VirtualSize;
    if((dwAddr >= pSecHeader[i].PointerToRawData) && (dwAddr <= pSecHeader[i].PointerToRawData + dwSecSize))
    {
     //3.�ҵ��ø�ƫ�ƣ����ڴ�ƫ�� = ��������ڴ�ƫ�� + ����ƫ�� - ��������ļ�ƫ�ƣ�
     dwRet = pSecHeader[i].VirtualAddress + dwAddr - pSecHeader[i].PointerToRawData;
    }
   }
  }
 }
 
 //5.�ͷ��ڴ�
 delete lpBase;
 lpBase = NULL;
 return dwRet;
}