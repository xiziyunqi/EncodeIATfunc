#include <stdio.h>
/*
Purpose:PE文件的内存偏移与文件偏移相互转换,不考虑系统为对齐填充偏移转换
szFileName:文件名
dwAddr:需要转换的偏移值
bFile2RVA:是否是文件偏移到内存偏移的转换，1 - dwAddr代表的是文件偏移，此函数返回内存偏移
    0 - dwAddr代表的是内存偏移，此函数返回文件偏移
返回值：相对应的偏移值,失败返回-1
*/

DWORD AddressConvert(char szFileName[], DWORD dwAddr, BOOL bFile2RVA)
{
 char *lpBase = NULL;
 DWORD dwRet = -1;
 //1.首先将文件读入内存
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

 //2.读取该文件的信息（文件内存对齐方式以及区块数量，并将区块表指针指向区块表第一个区块头）
 PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpBase;
 PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((unsigned long)lpBase + pDosHeader->e_lfanew);
 
 DWORD dwMemAlign = pNtHeader->OptionalHeader.SectionAlignment;
 DWORD dwFileAlign = pNtHeader->OptionalHeader.FileAlignment;
 int dwSecNum = pNtHeader->FileHeader.NumberOfSections;
 PIMAGE_SECTION_HEADER pSecHeader = (PIMAGE_SECTION_HEADER)((char *)lpBase + pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS));
 DWORD dwHeaderSize = 0;
 
 if(!bFile2RVA)  // 内存偏移转换为文件偏移
 { 
  //看需要转移的偏移是否在PE头内，如果在则两个偏移相同
  dwHeaderSize = pNtHeader->OptionalHeader.SizeOfHeaders;
  if(dwAddr <= dwHeaderSize)
  {
   delete lpBase;
   lpBase = NULL;
   return dwAddr;
  }
  else //不再PE头里，查看该地址在哪个区块中
  {
   for(int i = 0; i < dwSecNum; i++)
   {
    DWORD dwSecSize = pSecHeader[i].Misc.VirtualSize;
    if((dwAddr >= pSecHeader[i].VirtualAddress) && (dwAddr <= pSecHeader[i].VirtualAddress + dwSecSize))
    {
     //3.找到该该偏移，则文件偏移 = 该区块的文件偏移 + （该偏移 - 该区块的内存偏移）
     dwRet = pSecHeader[i].PointerToRawData + dwAddr - pSecHeader[i].VirtualAddress;
    }
   }
  }
 }
 else // 文件偏移转换为内存偏移
 {
  dwHeaderSize = pNtHeader->OptionalHeader.SizeOfHeaders;
  //看需要转移的偏移是否在PE头内，如果在则两个偏移相同
  if(dwAddr <= dwHeaderSize)
  {
   delete lpBase;
   lpBase = NULL;
   return dwAddr;
  }
  else//不再PE头里，查看该地址在哪个区块中
  {
   for(int i = 0; i < dwSecNum; i++)
   {
    DWORD dwSecSize = pSecHeader[i].Misc.VirtualSize;
    if((dwAddr >= pSecHeader[i].PointerToRawData) && (dwAddr <= pSecHeader[i].PointerToRawData + dwSecSize))
    {
     //3.找到该该偏移，则内存偏移 = 该区块的内存偏移 + （该偏移 - 该区块的文件偏移）
     dwRet = pSecHeader[i].VirtualAddress + dwAddr - pSecHeader[i].PointerToRawData;
    }
   }
  }
 }
 
 //5.释放内存
 delete lpBase;
 lpBase = NULL;
 return dwRet;
}