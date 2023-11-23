#include "Body.hpp"
#include <iostream>

/************************************************************
 *					CONSTRUCTORS/DESTRUCTOR					*
 ***********************************************************/

Body::Body(): _lastChunk(false), _length(0), _received(0), _chunkExpected(CHUNK_SIZE), _state(NO_BODY)
{
	this->_chunkExpected = CHUNK_SIZE;
}

Body::Body(const Body &src)
{
	*this = src;
}

Body	&Body::operator=(const Body &src)
{
	if (this != &src)
	{
		this->_lastChunk = src._lastChunk;
		this->_length = src._length;
		this->_received = src._received;
		this->_maxBodySize = src._maxBodySize;
		this->_chunkExpected = src._chunkExpected;
		this->_state = src._state;
		this->_body = src._body;
		this->_hexa = src._hexa;
	}
	return (*this);
}

Body::~Body() {}

/************************************************************
 *						SETTERS/GETTERS						*
 ***********************************************************/

void Body::setChunked()
{
	this->_state = CHUNKED;
}

void Body::setContentLength(long length)
{
	this->_length = length;
	this->_state = CONTENT_LENGTH;
}

void Body::setMaxBodySize(long size)
{
	if (this->_state == CONTENT_LENGTH && size != -1 && this->_length > size)
		throw BodyTooLargeException();
	this->_maxBodySize = size;
}

std::string Body::getBody() const
{
	return (this->_body);
}

/************************************************************
 *							UTILS							*
 ***********************************************************/

static unsigned int	hstoi(std::string hexa)
{
	int		result = 0;
	int		power = 1;

	for (int i = hexa.size() - 1; i >= 0; i--)
	{
		if (hexa[i] >= '0' && hexa[i] <= '9')
			result += (hexa[i] - '0') * power;
		else if (hexa[i] >= 'a' && hexa[i] <= 'f')
			result += (hexa[i] - 'a' + 10) * power;
		else if (hexa[i] >= 'A' && hexa[i] <= 'F')
			result += (hexa[i] - 'A' + 10) * power;
		power *= 16;
	}
	return (result);
}

void Body::parseLine(const std::string line)
{
	if (this->_state == NO_BODY || this->_state == FINISHED)
		return;
	if (this->_state == CONTENT_LENGTH)
	{
		for (unsigned int i = 0; i < line.size(); i++)
		{
			this->_body.push_back(line[i]);
			this->_received++;
			if (this->_received == this->_length)
			{
				this->_state = FINISHED;
				return;
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < line.size(); i++)
		{
			if (this->_chunkExpected == CHUNK_END_R)
			{
				if (line[i] != '\r')
					throw BodyException();
				this->_chunkExpected = CHUNK_END_N;
			}
			else if (this->_chunkExpected == CHUNK_END_N)
			{
				if (line[i] != '\n')
					throw BodyException();
				if (this->_lastChunk)
				{
					this->_state = FINISHED;
					return;
				}
				if (this->_hexa.size() == 0)
					this->_chunkExpected = CHUNK_SIZE;
				else
				{
					this->_length = hstoi(this->_hexa);
					this->_received = 0;
					this->_hexa.clear();
					if (this->_length == 0)
					{
						this->_chunkExpected = CHUNK_END_R;
						this->_lastChunk = true;
					}
					else
						this->_chunkExpected = CHUNK_DATA;
				}
			}
			else if (this->_chunkExpected == CHUNK_SIZE)
			{
				if (line[i] == '\r')
				{
					this->_chunkExpected = CHUNK_END_N;
					continue;
				}
				if (!isHexa(line[i]))
					throw BodyException();
				this->_hexa.push_back(line[i]);
			}
			else if (this->_chunkExpected == CHUNK_DATA)
			{
				this->_body.push_back(line[i]);
				this->_received++;
				if (this->_maxBodySize != -1 && this->_received >= this->_maxBodySize)
					throw BodyTooLargeException();
				if (this->_received == this->_length)
					this->_chunkExpected = CHUNK_END_R;
			}
		}
	}
}

bool Body::isFinished() const
{
	return (this->_state == FINISHED);
}

static bool	isHexa(char c)
{
	return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

void Body::clear()
{
	this->_lastChunk = false;
	this->_length = 0;
	this->_received = 0;
	this->_maxBodySize = 0;
	this->_chunkExpected = CHUNK_SIZE;
	this->_state = NO_BODY;
	this->_body.clear();
	this->_hexa.clear();
}

