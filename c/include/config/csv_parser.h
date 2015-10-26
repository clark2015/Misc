
#ifndef CSV_PARSER_HPP_INCLUDED

#define CSV_PARSER_HPP_INCLUDED

#define LIBCSV_PARSER_MAJOR_VERSION 1

#define LIBCSV_PARSER_MINOR_VERSION 0

#define LIBCSV_PARSER_PATCH_VERSION 0

#define LIBCSV_PARSER_VERSION_NUMBER 10000

/* C++ header files */
#include <string>
#include <vector>
#include <map>

/* C header files */
#include <cstdio>
#include <cstring>
#include <cstdlib>

long long atoll_noerror(const char *nptr);

class field
{
public:
    field( ) {}
    field(const std::string& value) : m_value(value) {}
	field(const char*p,size_t len)  : m_value(p,len) {}
	const std::string GetString() const	{	return m_value;	}
    float GetFloat() const { return static_cast<float>(atof(m_value.c_str())); }
    operator const unsigned int() 	const   { return (unsigned int) (GetFloat()+0.5); }
    operator const unsigned long() 	const   { return (unsigned long)(GetFloat()+0.5); }
    operator const int() 			const   { return int(GetFloat()); }
    operator const long() 			const   { return int(GetFloat()); }
    operator const unsigned short() const   { return (unsigned short)(GetFloat()+0.5); }
    operator const short() 			const	{ return short(GetFloat()+0.5); }
    operator const unsigned char() 	const   { return (unsigned char)(GetFloat()+0.5); }
    operator const char() 			const    { return char(GetFloat()+0.5); }
    operator const bool() 	const     	{ return atoi(m_value.c_str()) != 0; }
    operator const float() 	const    	{ return GetFloat(); }
    operator const double() const   	{ return atof(m_value.c_str()); }
    operator const char*() 	const    	{ return m_value.c_str(); }

	//#if	defined(_ISOC99_SOURCE) || defined(atoll)         vc6 没有long long，现在不考虑vc6，直接提出来使用
	operator const			long long() const	{ return atoll_noerror(m_value.c_str()); }		//不用+0.5吧
	operator const unsigned long long() const	{ return atoll_noerror(m_value.c_str()); }
	//#endif
protected:
	const std::string m_value;
};

typedef std::vector <std::string> vt_csv_row;
typedef std::map <std::string, field> map_csv_row;
typedef vt_csv_row * vt_csv_row_ptr;

class csv_parser
{
public :
	csv_parser() : enclosed_char(0x00),
				   field_term_char('\t'),  	line_term_char(0x0A),
                   record_count(0U),		input_fp(NULL),
				   more_rows(false)
				   { }

	~csv_parser()
	{
        if(input_fp)
        {
		    fclose(input_fp);
            input_fp = NULL;
        }
	}


    bool init(FILE * input_file_pointer)
    {
	    input_fp = input_file_pointer;

	    if (input_fp == NULL)
	    {
		    fprintf(stderr, "Fatal error : unable to open input file from file pointer\n");

		    return false;
	    }

	    /* Resetting the internal pointer to the beginning of the stream */
	    rewind(input_fp);

	    more_rows = true;

	    _get_header();

	    return true;
    }

    bool init(const char * input_file)
    {
    #ifdef _WIN32
	    fopen_s(&input_fp, input_file, "rb");
	#else
		input_fp = fopen(input_file, "rb");
	#endif

	    if (input_fp == NULL)
	    {
		    fprintf(stderr, "Fatal error : unable to open input file %s\n", input_file);

		    return false;
	    }

	    more_rows = true;

	    _get_header();

	    return true;
    }

    void get_row(vt_csv_row& current_row)
    {
        current_row.clear();

	    /* This will store the length of the buffer */
	    unsigned int line_length = 0U;

	    /* Character array buffer for the current record */
	    char * line = NULL;

	    /* Grab one record */
	    _read_single_line(&line, &line_length);

	    _get_fields_with_optional_enclosure(&current_row, line, &line_length);

	    /* Deallocate the current buffer */
        free(line);
        line = NULL;

	    /* Keeps track of how many times this has method has been called */
	    record_count++;
    }

    void get_map_row(map_csv_row& current_row, const vt_csv_row& row)
    {
        current_row.clear();

        if(row.size() != header_row.size())
        {
            return;
        }

        for(unsigned int i = 0; i < header_row.size(); ++i)
        {
            current_row.insert(std::pair<std::string, std::string>(header_row[i], row[i]));
        }
    }

	bool has_more_rows(void)
	{
		return more_rows;
	}

	unsigned int get_record_count(void)
	{
		return record_count;
	}

	void reset_record_count(void)
	{
		record_count = 0U;
	}

private :
    
    void _get_header(void)
    {
	    if(has_more_rows())
	    {
		    get_row(header_row);
	    }

	    record_count = 0U;
    }

    void _get_fields_with_optional_enclosure(vt_csv_row_ptr row, const char * line, const unsigned int * line_length)
    {
	    char * field = NULL;

	    /*
	     * How to extract the fields, when the enclosure char is optional.
	     *
	     * This is very similar to parsing the document without enclosure but with the following conditions.
	     *
	     * If the beginning char is an enclosure character, adjust the starting position of the string by + 1.
	     * If the ending char is an enclosure character, adjust the ending position by -1
	     */
	    if (*line_length > 0)
	    {
		    field = (char *) malloc(*line_length);

		    memset(field, 0, *line_length);

		    register unsigned int field_start   = 0U;
		    register unsigned int field_end     = 0U;
		    register unsigned int char_pos 		= 0U;

		    while(char_pos < *line_length)
		    {
			    char curr_char = line[char_pos];

			    if (curr_char == field_term_char)
			    {
				    field_end = char_pos;

				    const char * field_starts_at = line + field_start;

				    /* Field width must exclude field delimiter characters */
				    unsigned int field_width = field_end - field_start;

				    const char line_first_char = field_starts_at[0];
				    const char line_final_char = field_starts_at[field_width - 1];

				    /* If the enclosure char is found at either ends of the string */
				    unsigned int first_adjustment = (line_first_char == enclosed_char) ? 1U : 0U;
				    unsigned int final_adjustment = (line_final_char == enclosed_char) ? 2U : 0U;

				    /* We do not want to have any negative or zero field widths */
				    field_width = (field_width > 2U) ? (field_width - final_adjustment) : field_width;

				    /* Copy exactly field_width bytes from field_starts_at to field */
				    memcpy(field, field_starts_at + first_adjustment, field_width);

				    /* This must be a null-terminated character array */
				    field[field_width] = 0x00;

				    std::string field_string_obj = field;

				    row->push_back(field_string_obj);

				    /* This is the starting point of the next field */
				    field_start = char_pos + 1;

			    } else if (curr_char == line_term_char)
			    {
				    field_end = char_pos;

				    const char * field_starts_at = line + field_start;

				    /* Field width must exclude line terminating characters */
				    unsigned int field_width = field_end - field_start;

				    const char line_first_char = field_starts_at[0];
				    const char line_final_char = field_starts_at[field_width - 1];

				    /* If the enclosure char is found at either ends of the string */
				    unsigned int first_adjustment = (line_first_char == enclosed_char) ? 1U : 0U;
				    unsigned int final_adjustment = (line_final_char == enclosed_char) ? 2U : 0U;

				    /* We do not want to have any negative or zero field widths */
				    field_width = (field_width > 2U) ? (field_width - final_adjustment) : field_width;

				    /* Copy exactly field_width bytes from field_starts_at to field */
				    memcpy(field, field_starts_at + first_adjustment, field_width);

				    /* This must be a null-terminated character array */
				    field[--field_width] = 0x00;

				    std::string field_string_obj = field;

				    row->push_back(field_string_obj);
			    }

			    /* Move to the next character in the current line */
			    char_pos++;
		    }

		    /* Deallocate memory for field buffer */
            free(field);
            field = NULL;
	    }
    }

    void _read_single_line(char ** buffer, unsigned int * buffer_len)
    {
	    long int original_pos = ftell(input_fp);
	    long int current_pos  = original_pos;

	    register int current_char = 0;

	    /* Checking one character at a time until the end of a line is found */
	    while(true)
	    {
		    current_char = fgetc(input_fp);

		    if (current_char == EOF)
		    {
			    /* We have reached the end of the file */
			    more_rows = false;

			    break;

		    } else if (current_char == line_term_char)
		    {
			    /* We have reached the end of the row */
			    current_pos++;

			    break;

		    } else {

			    current_pos++;
		    }
	    }

	    /* Let's try to peek one character ahead to see if we are at the end of the file */
	    if (more_rows)
	    {
		    current_char = fgetc(input_fp);

		    more_rows = (current_char == EOF) ? false : true;
	    }

	    /* Find out how long this row is */
	    const size_t length_of_row = current_pos - original_pos;

	    if (length_of_row > 0)
	    {
		    *buffer_len = length_of_row * sizeof(char) + 1;

		    *buffer = (char *) realloc(*buffer, *buffer_len);

		    memset(*buffer, 0, *buffer_len);

		    /* Reset the internal pointer to the original position */
		    fseek(input_fp, original_pos, SEEK_SET);

		    /* Copy the contents of the line into the buffer */
		    fread(*buffer, 1, length_of_row, input_fp);
	    }
    }

protected :
	char enclosed_char;
	char field_term_char;
	char line_term_char;
	unsigned int record_count;
	FILE * input_fp;
	bool more_rows;
    vt_csv_row header_row;

};

#endif /* CSV_PARSER_HPP_INCLUDED */
