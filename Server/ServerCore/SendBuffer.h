#pragma once

class SendBuffer
{
public:
	SendBuffer(uint32 allocSize);
	~SendBuffer();

	BYTE*		Buffer() { return _buffer.data(); }
	uint32		AllocSize() { return _allocSize; }
	uint32		WriteSize() { return _writeSize; }
	void		Close(uint32 writeSize);

private:
	vector<BYTE>		_buffer;
	uint32				_allocSize = 0;
	uint32				_writeSize = 0;
};


