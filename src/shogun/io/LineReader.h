/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Evgeniy Andreev, Thoralf Klein, Heiko Strathmann, Yuyu Zhang, 
 *          Fernando Iglesias, Bjoern Esser, Soeren Sonnenburg, Saurabh Goyal
 */

#ifndef __LINE_READER_H__
#define __LINE_READER_H__

#include <shogun/lib/config.h>

#include <shogun/lib/SGVector.h>
#include <shogun/base/SGObject.h>

namespace shogun
{
class CCircularBuffer;
class CTokenizer;

/** @brief Class for buffered reading from a ascii file */
class SHOGUN_EXPORT CLineReader : public CSGObject
{
public:
	/** default constructor */
	CLineReader();

	/** create object associated with the stream to read
	 *
	 * @param stream readable stream
	 * @param tokenizer enabling to parse different ascii file formats (.csv, ...)
	 */
	CLineReader(FILE* stream, CTokenizer* tokenizer);

	/** create object associated with the stream to read
	 * and specify maximum length of a string that can be read
	 *
	 * @param max_string_length the maximum string length a line is allowed to have
	 * @param stream readable stream
	 * @param tokenizer enabling to parse different ascii file formats (.csv, ...)
	 */
	CLineReader(int32_t max_string_length, FILE* stream, CTokenizer* tokenizer);

	/** deconstructor */
	virtual ~CLineReader();

	/** check for next line in the stream
	 *
	 * @return true if there is next line, false - otherwise
	 */
	virtual bool has_next();

	/** skip next line */
	virtual void skip_line();

	/** read string	*/
	virtual SGVector<char> read_line();

	/** set position of stream to the beginning and clear buffer */
	void reset();

	/** set tokenizer
	 *
	 * @param tokenizer tokenizer
	 */
	void set_tokenizer(CTokenizer* tokenizer);

	/** @return object name */
	virtual const char* get_name() const { return "LineReader"; }

private:
	/** class initialization */
	void init();

	/** read file into memory */
	int32_t read(int32_t& bytes_to_skip);

	/** read token from internal buffer */
	SGVector<char> read_token(int32_t line_len);

private:
	/** internal buffer for searching */
	CCircularBuffer* m_buffer;

	/** */
	CTokenizer* m_tokenizer;

	/** readable stream */
	FILE* m_stream;

	/** maximum length of a line that can be read */
	int32_t m_max_token_length;

	/** length of next line in the buffer */
	int32_t m_next_token_length;
};

}

#endif /* __FILE_READER_H__ */
