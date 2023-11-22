#pragma once

#include <string>

class Body
{
private:
	enum BodyState {
	CHUNKED,
	CONTENT_LENGTH,
	FINISHED,
	NO_BODY,
	};

	enum ChunkExpected {
		CHUNK_SIZE,
		CHUNK_DATA,
		CHUNK_END_R,
		CHUNK_END_N,
	};

	bool			_lastChunk;
	unsigned int	_length;
	unsigned int	_received;
	unsigned int	_maxBodySize;
	ChunkExpected	_chunkExpected;
	BodyState		_state;
	std::string		_body;
	std::string		_hexa;

public:
	Body();
	~Body();
	Body(const Body &src);
	Body	&operator=(const Body &src);

	void		parseLine(const std::string line);
	std::string	getBody() const;
	bool		isFinished() const;
	void		clear();
	void		setChunked();
	void		setContentLength(unsigned int length);
	void		setMaxBodySize(unsigned int size);

	class BodyException: public std::exception {
		virtual const char *what() const throw() {
			return ("Body exception");
		}
	};

	class BodyTooLargeException: public std::exception {
		virtual const char *what() const throw() {
			return ("Body too large");
		}
	};
};
