/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Evgeniy Andreev, Thoralf Klein, Viktor Gal, Bjoern Esser, 
 *          Heiko Strathmann
 */

#include <shogun/io/LineReader.h>
#include <shogun/lib/CircularBuffer.h>
#include <shogun/lib/Tokenizer.h>
#include <shogun/io/SGIO.h>
#include <cstdio>

using namespace shogun;

CLineReader::CLineReader()
{
	init();

	m_buffer=std::make_shared<CCircularBuffer>();
}

CLineReader::CLineReader(FILE* stream, std::shared_ptr<CTokenizer> tokenizer)
{
	init();

	m_stream=stream;
	m_max_token_length=10*1024*1024;

	
	m_tokenizer=tokenizer;

	m_buffer=std::make_shared<CCircularBuffer>(m_max_token_length);
	m_buffer->set_tokenizer(m_tokenizer);
}

CLineReader::CLineReader(int32_t max_token_length, FILE* stream, std::shared_ptr<CTokenizer> tokenizer)
{
	init();

	m_stream=stream;
	m_max_token_length=max_token_length;

	
	m_tokenizer=tokenizer;

	m_buffer=std::make_shared<CCircularBuffer>(m_max_token_length);
	m_buffer->set_tokenizer(m_tokenizer);
}

CLineReader::~CLineReader()
{
	
	
}

bool CLineReader::has_next()
{
	if (m_stream==NULL || m_max_token_length==0 || m_tokenizer==NULL)
	{
		SG_ERROR("CLineReader::has_next():: Class is not initialized\n");
		return false;
	}

	if (ferror(m_stream))
	{
		SG_ERROR("CLineReader::has_next():: Error reading file\n");
		return false;
	}

	if (feof(m_stream) && (m_buffer->num_bytes_contained()<=0 || !m_buffer->has_next()))
		return false; // nothing to read

	return true;
}

void CLineReader::skip_line()
{
	int32_t bytes_to_skip=0;
	m_next_token_length=read(bytes_to_skip);
	if (m_next_token_length==-1)
		return;
	else
		m_buffer->skip_characters(bytes_to_skip);
}

SGVector<char> CLineReader::read_line()
{
	SGVector<char> line;

	int32_t bytes_to_skip=0;
	m_next_token_length=read(bytes_to_skip);
	if (m_next_token_length==-1)
		line=SGVector<char>();
	else
	{
		m_buffer->skip_characters(bytes_to_skip);
		line=read_token(m_next_token_length-bytes_to_skip);
	}

	return line;
}

void CLineReader::reset()
{
	rewind(m_stream);
	m_buffer->clear();
}

void CLineReader::set_tokenizer(std::shared_ptr<CTokenizer> tokenizer)
{
	
	
	m_tokenizer=tokenizer;

	m_buffer->set_tokenizer(tokenizer);
}

void CLineReader::init()
{
	m_buffer=NULL;
	m_tokenizer=NULL;
	m_stream=NULL;

	m_max_token_length=0;
	m_next_token_length=-1;
}

int32_t CLineReader::read(int32_t& bytes_to_skip)
{
	int32_t line_end=0;
	int32_t bytes_to_read=0;
	int32_t temp_bytes_to_skip=0;

	while (1)
	{
		if (bytes_to_skip==line_end)
			line_end=m_buffer->next_token_idx(bytes_to_skip);
		else
			line_end=m_buffer->next_token_idx(temp_bytes_to_skip);

		if (m_buffer->num_bytes_contained()!=0 && line_end<m_buffer->num_bytes_contained())
			return line_end;
		else if (m_buffer->available()==0)
			return -1; // we need some limit in case file does not contain delimiter

		// if there is no delimiter in buffer
		// try get more data from stream
		// and write it into buffer
		if (m_buffer->available() < m_max_token_length)
			bytes_to_read=m_buffer->available();
		else
			bytes_to_read=m_max_token_length;

		if (feof(m_stream))
			return line_end;
		else
			m_buffer->push(m_stream, bytes_to_read);

		if (ferror(m_stream))
		{
			SG_ERROR("CLineReader::read(int32_t&):: Error reading file\n");
			return -1;
		}
	}
}

SGVector<char> CLineReader::read_token(int32_t line_len)
{
	SGVector<char> line;

	if (line_len==0)
		line=SGVector<char>();
	else
		line=m_buffer->pop(line_len);

	return line;
}
