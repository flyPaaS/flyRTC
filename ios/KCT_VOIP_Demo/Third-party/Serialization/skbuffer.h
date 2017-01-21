#pragma once

namespace Comm
{
    
    typedef struct tagSKBufferImpl SKBufferImpl_t;
    
    class SKBuffer
    {
    public:
        SKBuffer();
        ~SKBuffer();
        
        int EnsureExpandSize( int iWriteSize );
        
        unsigned char * GetWritePtr( int iWriteSize );
        
        void AddWritePos( int iWritePos );
        
        void SetWritePos( int iWritePos );
        
        int Write( const void * sBuffer, int iLen );
        
        int Printf( const char * sFmt, ... );
        
        unsigned char * GetReadPtr( int iReadSize );
        
        void AddReadPos( int iReadPos );
        
        void SetReadPos( int iReadPos );
        
        /* size has been read */
        int GetReadSize();
        
        unsigned char * GetBuffer();
        
        void Reset();
        
        int GetLen();
        
        int GetCapacity();
        //*********************************************
        unsigned char * GetBuffer()const;
        
        int GetLen()const;
        
        int GetCapacity()const;
        //*********************************************
        
        unsigned char * Detach( int * piLen );
        
        void Attach( unsigned char * sBuffer, int iLen );
        
        void AttachMalloc( unsigned char * sBuffer, int iLen );
        
        /* save buffer to file */
        /* @ret 0 -- ok, <>0 -- failed */
        int SaveToFile(const char *sFile);
        
    private:
        SKBuffer(const SKBuffer& rhs);
        SKBuffer & operator = (SKBuffer &rhs);
        
    private:
        SKBufferImpl_t * m_ptBufferImpl;
    };
    
    
    template<typename N, class T>
    class SKSmartPickleStruct : public T
    {
    public:
        SKSmartPickleStruct()
        {
            memset( (T*)this, 0, sizeof( T ) );
        }
        
        SKSmartPickleStruct( const T & other )
        {
            memset( (T*)this, 0, sizeof( T ) );
            N::DeepCopy( (T*)this, &other );
        }
        
        SKSmartPickleStruct( const SKSmartPickleStruct & other )
        {
            memset( (T*)this, 0, sizeof( T ) );
            N::DeepCopy( (T*)this, &other );
        }
        
        SKSmartPickleStruct & operator=( const T & other )
        {
            if( this != &other )
            {
                N::FreeStructField( *this );
                
                N::DeepCopy( (T*)this, &other );
            }
            
            return *this;
        }
        
        ~SKSmartPickleStruct()
        {
            N::FreeStructField( *this );
        }
    };
    
}

