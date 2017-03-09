#pragma once

#include <memory>
#include <functional>
#include <cstdlib>
#include <cstdarg>
#include <zmq.h>


namespace zmq {

/** \brief ZeroMQ error
 *
 * Exception to identify ZeroMQ errors.  The constructor has no
 * arguments as ZeroMQ handles errors using a global variable which is
 * evaluated using zmq_errno() inside the exception.
 *
 * \code
 * try
 * {
 *   // code with zeromq
 * }
 * catch (zmq::error &e)
 * {
 *   // handle exception
 * }
 * \endcode
 */
class error : public std::exception
{
private:
  int errnum;
public:
  /** \brief Constructor */
  error() : errnum(zmq_errno()) {}

  char const * what() const noexcept override
  {
    return zmq_strerror(errnum);
  }

  /** \brief %what() function
   *
   * \returns error message
   */
  int num() const noexcept
  {
    return errnum;
  }
};


/** \brief Socket class
 *
 * The socket class exposes parts of the ZeroMQ C API in an
 * object-oriented manner.  Only the functionality we require is
 * implemented.  The socket object implicitly behaves like a pointer
 * to void which is what ZeroMQ expects.  Thus a socket object can be
 * used like a regular %socket pointer with the C API.  For example
 * the `zmq_sendmsg` is not implemented but can be used nevertheless.
 * \code
 * if ( zmq_sendmsg(socket, msg, flags) == -1 )
 *   throw zmq::error();
 * \endcode
 * When using the C API directly, errors have to be checked explicitly!
 *
 * For details see http://api.zeromq.org/.
 */
class socket
{
  std::unique_ptr < void, std::function < void(void*) > > m_socket;
public:
  /** \brief Constructor
   *
   * The constructor is not meant to be called by itself but is rather
   * used in the context object to return a new socket.
   *
   * \param[in] new_socket    pre-allocated ZeroMQ socket
   * \throws zmq::error
   */
  explicit socket(void * new_socket) throw(zmq::error)
    : m_socket( new_socket, zmq_close )
  {
    if ( !new_socket )
      throw error();
  }

  /** \brief Conversion operator
   *
   * A socket object implicitly behaves like a pointer to void which
   * is what ZeroMQ expects, so that a socket object can be used like
   * a regular %socket pointer with the C API.
   */
  operator void *() const noexcept { return m_socket.get(); }

  /** \brief Accept incoming connections on a socket.
   *
   * The bind() function binds the socket to a local endpoint and
   * then accepts incoming connections on that endpoint.
   *
   * \param[in] endpoint   string with transport protocal and address
   * \throws zmq::error
   */
  void bind(char const *endpoint) const throw(zmq::error)
  {
    if ( zmq_bind(*this, endpoint) == -1 )
      throw error();
  }

  /** \brief Create outgoing connection from socket
   *
   * The connect() function connects the socket to an endpoint and
   * then accepts incoming connections on that endpoint.
   *
   * \param[in] endpoint   string with transport protocal and address
   * \throws zmq::error
   */
  void connect(char const *endpoint) const throw(zmq::error)
  {
    if ( zmq_connect(*this, endpoint) == -1 )
      throw error();
  }

  /** \brief Send a message part on a socket
   *
   * The send() function shall queue a message created from its
   * printf-style arguments.
   *
   * \param[in] fmt   printf-style string for argument formatting
   * \param[in] ...   variadic arguments
   */
  void send(char const *fmt, ...) const __attribute__((format (printf, 2, 3)));
  void send(char const *fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    char *tmp = nullptr;
    int len = vasprintf(&tmp, fmt, args);
    if ( len == -1 )
    {
      std::free(tmp);
      throw std::bad_alloc();
    }
    if ( zmq_send(*this, tmp, len+1, 0) == -1 )
    {
      std::free(tmp);
      throw error();
    }
    std::free(tmp);
  }
};


/** \brief Context class
 *
 * The context class exposes parts of the ZeroMQ C API in an
 * object-oriented manner.  Only the functionality we require is
 * implemented.  The context object implicitly behaves like a pointer
 * to void which is what ZeroMQ expects.  Thus a context object can be
 * used like a regular %context pointer with the C API.  For example
 * the `zmq_ctx_set` is not implemented but can be used nevertheless.
 * \code
 * if ( zmq_ctx_set(context, ZMQ_IPV6, 1) == -1 )
 *   throw zmq::error();
 * \endcode
 * When using the C API directly, errors have to be checked explicitly!
 *
 * For details see http://api.zeromq.org/.
 */
class context
{
  std::unique_ptr < void, std::function < int(void*) > > m_context;
public:
  /** \brief Constructor */
  context() noexcept : m_context( zmq_ctx_new(), zmq_ctx_term ) {}

  /** \brief Conversion operator
   *
   * A socket object implicitly behaves like a pointer to void which
   * is what ZeroMQ expects, so that a socket object can be used like
   * a regular %socket pointer with the C API.
   */
  operator void *() const noexcept { return m_context.get(); }

  /** Create ZeroMQ socket
   *
   * The socket() function shall create a ZeroMQ socket within the
   * context and return an opaque handle to the newly created socket.
   *
   * \param[in] type   specifies the socket type
   */
  zmq::socket socket(int type) const
  {
    /** errors of `zmq_socket` are handled in the zmq::socket constructor */
    return zmq::socket( zmq_socket(*this, type) );
  }
};

} // namespace zmq
