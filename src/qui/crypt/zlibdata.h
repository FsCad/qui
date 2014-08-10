#pragma once

#include "QBuffer.h"

class  ZlibData
{
public:
    ZlibData(LPCBYTE pData=NULL,DWORD nLen=0);
    // ��������������óɹ�,��Ҫʹ��delete ɾ��poutָ��ָ����ڴ�
    static BOOL CompressData(__in QBuffer &in,__out QBuffer &out);
    static BOOL CompressData( __in LPCBYTE pBufIn,__in DWORD nByteIn, __out QBuffer& bufOut );
    static BOOL DecompressData(__in QBuffer &in,__in DWORD nOrgSize, __out QBuffer &out);
    static BOOL DecompressData(__in LPCBYTE pin,__in DWORD nByteIn,__in DWORD nOrgSize, __out QBuffer &out);
    static BOOL DecompressData(__inout QBuffer &inout,__inout DWORD& nOrgSize);
public:
    BOOL ReadFile(LPCTSTR pszFile);
    // ����Ϊ����ѹ�������ݣ�ǰ�ĸ��ֽ�Ϊ��ԭʼ��С
    BOOL WriteDataFile(LPCTSTR pszFile);
    BOOL Compress();
    // param : ѹ��ǰ�����ݴ�С
    BOOL Decompress(DWORD nOrignDataLen);

    void SetData(LPCBYTE pData,DWORD nLen);
    void SetData(QBuffer &buf);
    void ClearData(){ m_buf.ClearBuffer();};
    QBuffer& GetData(){ return m_buf;};

private:
    QBuffer		m_buf;
    DWORD		m_dwOrignBufferLen;	// ѹ��ǰ�����ݴ�С�����ڽ�ѹ
};
