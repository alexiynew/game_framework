/// @file
/// @brief The realisation details of the logging system.
/// @author Fedorov Alexey
/// @date 03.03.2018

#ifndef FRAMEWORK_LOG_LOG_DETAILS_HPP
#define FRAMEWORK_LOG_LOG_DETAILS_HPP

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace framework
{
namespace log
{
enum class severity_level;

namespace log_details
{
/// @brief Custom stream buffer
class log_buffer : public std::streambuf
{
public:
    log_buffer(severity_level level, std::string tag);

    ~log_buffer() override;

    log_buffer(const log_buffer&) = delete;
    log_buffer& operator=(const log_buffer&) = delete;

    log_buffer(log_buffer&& other) noexcept;
    log_buffer& operator=(log_buffer&& other) noexcept;

protected:
    int overflow(int character) override;
    int sync() override;

private:
    severity_level m_level;
    std::string m_tag;
    std::vector<char_type> m_buffer;

    void reset_pointers();
    void flush_buffer();
};

/// @brief Custom output stream
class log_ostream : public std::ostream
{
public:
    explicit log_ostream(std::unique_ptr<std::streambuf> buffer);

    ~log_ostream() override;

    log_ostream(const log_ostream&) = delete;
    log_ostream& operator=(const log_ostream&) = delete;

    log_ostream(log_ostream&& other) noexcept;
    log_ostream& operator=(log_ostream&& other) noexcept;

private:
    std::unique_ptr<std::streambuf> m_buffer;
};

} // namespace log_details

/// @}

} // namespace log

} // namespace framework

#endif
