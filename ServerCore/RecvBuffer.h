#pragma once
class RecvBuffer
{
private:
	int bufferSize = 0;
	int readPos = 0;
	int writePos = 0;
	vector<BYTE> buffer;
public:
	RecvBuffer();
	~RecvBuffer();
public:
	BYTE* ReadPos() { return &buffer[readPos]; }
	BYTE* WritePos() { return &buffer[writePos]; }
	int DataSize() const { return writePos - readPos; }
};

