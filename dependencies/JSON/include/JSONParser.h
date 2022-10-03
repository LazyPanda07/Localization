#pragma once

#include <istream>

#include "JSONUtility.h"

#include "Exceptions/CantFindValueException.h"
#include "Exceptions/WrongInputStreamException.h"

namespace json
{
	/// @brief Parser for JSON
	class JSON_API JSONParser
	{
	public:
		/// @brief std::variant specialization for JSON
		using variantType = utility::jsonObject::variantType;
		
		/// @brief JSON object
		using objectType = utility::jsonObject;

	public:
		/// @brief Iterator through JSONParser
		class JSON_API ConstIterator
		{
		private:
			using ConstIteratorType = std::vector<std::pair<std::string, variantType>>::const_iterator;

		private:
			ConstIteratorType begin;
			ConstIteratorType end;
			ConstIteratorType current;

		public:
			ConstIterator() = default;

			ConstIterator(const ConstIterator& other);

			ConstIterator(ConstIteratorType begin, ConstIteratorType end, ConstIteratorType start);

			ConstIterator operator ++ (int) noexcept;

			const ConstIterator& operator ++ () noexcept;

			ConstIterator operator -- (int) noexcept;

			const ConstIterator& operator -- () noexcept;

			const ConstIteratorType& operator * () const noexcept;

			const ConstIteratorType& operator -> () const noexcept;

			bool operator == (const ConstIterator& other) const noexcept;

			bool operator != (const ConstIterator& other) const noexcept;

			~ConstIterator() = default;
		};

	private:
		std::string rawData;
		utility::jsonObject parsedData;

	private:
		static utility::jsonObject::variantType getValue(const std::string& value);

		static void insertKeyValueData(std::string&& key, const std::string& value, utility::jsonObject*& ptr, std::vector<utility::jsonObject>* currentArray);

		static std::pair<std::vector<std::pair<std::string, variantType>>::const_iterator, bool> find(const std::string& key, const std::vector<std::pair<std::string, variantType>>& start);

		static utility::jsonObject::variantType* findObject(const std::vector<utility::jsonObject>& currentArray);

		static bool isStringSymbol(char symbol);

		static char interpretEscapeSymbol(char symbol);

	private:
		void parse();

	public:
		JSONParser() = default;

		/// <summary>
		/// Parse data
		/// </summary>
		/// <param name="data">JSON formatted data</param>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		JSONParser(const std::string& data);

		/// @brief Parse data
		/// @param inputStream JSON formatted data from stream
		/// @exception json::exceptions::WrongEncodingException Can't convert JSON formatted string to UTF8 encoding
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		JSONParser(std::istream& inputStream);

		/// @brief Parse data
		/// @param inputStream JSON formatted data from stream
		/// @exception json::exceptions::WrongEncodingException Can't convert JSON formatted string to UTF8 encoding
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		JSONParser(std::istream&& inputStream);

		/// @brief Construct from parsed or builded data 
		/// @param data Data from JSONBuilder or JSONParser
		JSONParser(const utility::jsonObject& data);

		/// @brief Copy constructor
		/// @param other Other JSONParser
		JSONParser(const JSONParser& other);

		/// @brief Move constructor
		/// @param other Other JSONParser
		JSONParser(JSONParser&& other) noexcept;

		/// @brief Copy operator
		/// @param other Other JSONParser
		/// @return Self
		JSONParser& operator = (const JSONParser& other);

		/// @brief Move operator
		/// @param other Other JSONParser
		JSONParser& operator = (JSONParser&& other) noexcept;

		/// @brief Checks if there is a object with key equivalent to key in the container and type equivalent to type in the container
		/// @param key Object name
		/// @param type Object type
		bool contains(const std::string& key, utility::variantTypeEnum type) const;

		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted string</param>
		/// <param name="codepage">codepage of jsonData</param>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		void setJSONData(const std::string& jsonData, uint32_t codePage);

		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted UTF8 string</param>
		void setJSONData(const std::string& jsonData);

		/// <summary>
		/// Setter for rawData
		/// </summary>
		/// <param name="jsonData">JSON formatted UTF8 string</param>
		void setJSONData(std::string&& jsonData) noexcept;

		/// @brief Setter for rawData
		/// @param inputStream Stream that contains JSON formatted UTF8 string
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		void setJSONData(std::istream& inputStream);

		/// @brief Setter for rawData
		/// @param inputStream Stream that contains JSON formatted UTF8 string
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		void setJSONData(std::istream&& inputStream);

		/// <summary>
		/// Getter for rawData
		/// </summary>
		/// <returns>rawData</returns>
		const std::string& getRawData() const;

		/// <summary>
		/// Getter for rawData
		/// </summary>
		/// <returns>rawData</returns>
		const std::string& operator * () const;

		/// <summary>
		/// Get iterator to begin of the JSON parsedData
		/// </summary>
		/// <returns>begin iterator</returns>
		ConstIterator begin() const noexcept;

		/// <summary>
		/// Get iterator to end of the JSON parsedData
		/// </summary>
		/// <returns>end iterator</returns>
		ConstIterator end() const noexcept;

		/// <summary>
		/// <para>Getter for all JSON parsed values</para>
		/// <para>T is one of json::utility::jsonObject::variantType template parameters</para>
		/// <para>Find and get first value of given key</para>
		/// </summary>
		/// <typeparam name="T">T is one of JSONParser::jsonStruct::variantType template parameters</typeparam>
		/// <param name="key">JSON key</param>
		/// <returns>JSON value</returns>
		/// <exception cref="json::exceptions::CantFindValueException">can't find JSON value</exception>
		/// <exception cref="std::bad_variant_access">Other type found</exception>
		template<typename T>
		const T& get(const std::string& key) const;

		/// @brief Get null value. Find and get first value of given key
		/// @param key JSON key
		/// @return nullptr value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		nullptr_t getNull(const std::string& key) const;

		/// @brief Get string value. Find and get first value of given key
		/// @param key JSON key
		/// @return string value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const std::string& getString(const std::string& key) const;
		
		/// @brief Get bool value. Find and get first value of given key
		/// @param key JSON key
		/// @return bool value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		bool getBool(const std::string& key) const;

		/// @brief Get int64_t value. Find and get first value of given key
		/// @param key JSON key
		/// @return int64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		int64_t getInt(const std::string& key) const;

		/// @brief Get uint64_t value. Find and get first value of given key
		/// @param key JSON key
		/// @return uint64_t value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		uint64_t getUnsignedInt(const std::string& key) const;

		/// @brief Get double value. Find and get first value of given key
		/// @param key JSON key
		/// @return double value
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		double getDouble(const std::string& key) const;

		/// @brief Get JSON array. Find and get first value of given key
		/// @param key JSON key
		/// @return JSON array
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const std::vector<utility::jsonObject>& getArray(const std::string& key) const;

		/// @brief Get JSON object. Find and get first value of given key
		/// @param key JSON Key
		/// @return JSON object
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		const utility::jsonObject& getObject(const std::string& key) const;

		/// @brief Getter for parsedData
		/// @return parsedData
		const utility::jsonObject& getParsedData() const;

		/// @brief Move parsed data
		/// @param object Result of moving
		void getParsedData(utility::jsonObject& object) noexcept;

		/// <summary>
		/// Get JSON from input stream
		/// </summary>
		/// <param name="stream">std::istream subclass instance</param>
		/// <param name="parser">reference to JSONParser instance</param>
		/// <returns>inputStream</returns>
		/// <exception cref="json::exceptions::WrongEncodingException">can't convert JSON formatted string to UTF8 encoding</exception>
		/// @exception json::exceptions::WrongInputStreamException Can't read JSON formatted data from inputStream
		friend JSON_API std::istream& operator >> (std::istream& inputStream, JSONParser& parser);

		/// <summary>
		/// Set JSON to output stream
		/// </summary>
		/// <param name="outputStream">std::ostream subclass instance</param>
		/// <param name="parser">const reference to JSONParser instance</param>
		/// <returns>outputStream</returns>
		friend JSON_API std::ostream& operator << (std::ostream& outputStream, const JSONParser& parser);

		~JSONParser() = default;
	};
}
