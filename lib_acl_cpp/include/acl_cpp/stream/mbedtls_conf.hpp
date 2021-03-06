#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/thread_mutex.hpp"
#include "sslbase_conf.hpp"
#include <vector>

namespace acl
{

/**
 * SSL 证书校验级别类型定义
 */
typedef enum
{
	MBEDTLS_VERIFY_NONE,	// 不校验证书
	MBEDTLS_VERIFY_OPT,	// 选择性校验，可以在握手时或握手后校验
	MBEDTLS_VERIFY_REQ	// 要求在握手时校验
} mbedtls_verify_t;

class mbedtls_io;

/**
 * SSL 连接对象的配置类，该类对象一般可以声明为全局对象，用来对每一个 SSL
 * 连接对象进行证书配置；该类加载了全局性的证书、密钥等信息；每一个 SSL 对象
 * (mbedtls_io) 调用本对象的setup_certs 方法来初始化自身的证书、密钥等信息
 */
class ACL_CPP_API mbedtls_conf : public sslbase_conf
{
public:
	/**
	 * 构造函数
	 * @param server_side {bool} 用来指定是服务端还是客户端，当为 true 时
	 *  为服务端模式，否则为客户端模式
	 */
	mbedtls_conf(bool server_side);
	~mbedtls_conf(void);

	/**
	 * 加载 CA 根证书(每个配置实例只需调用一次本方法)
	 * @param ca_file {const char*} CA 证书文件全路径
	 * @param ca_path {const char*} 多个 CA 证书文件所在目录
	 * @return {bool} 加载  CA 根证书是否成功
	 * 注：如果 ca_file、ca_path 均非空，则会依次加载所有证书
	 */
	bool load_ca(const char* ca_file, const char* ca_path);

	/**
	 * 添加一个服务端/客户端自己的证书，可以多次调用本方法加载多个证书
	 * @param crt_file {const char*} 证书文件全路径，非空
	 * @return {bool} 添加证书是否成功
	 */
	bool add_cert(const char* crt_file);

	/**
	 * 添加服务端/客户端的密钥(每个配置实例只需调用一次本方法)
	 * @param key_file {const char*} 密钥文件全路径，非空
	 * @param key_pass {const char*} 密钥文件的密码，没有密钥密码可写 NULL
	 * @return {bool} 设置是否成功
	 */
	bool set_key(const char* key_file, const char* key_pass = NULL);

	/**
	 * 当为服务端模式时是否启用会话缓存功能，有助于提高 SSL 握手效率
	 * @param on {bool}
	 * 注：该函数仅对服务端模式有效
	 */
	void enable_cache(bool on);

	/**
	 * 获得随机数生成器的熵对象
	 * @return {void*}，返回值为 entropy_context 类型
	 */
	void* get_entropy(void)
	{
		return entropy_;
	}

	/**
	 * mbedtls_io::open 内部会调用本方法用来安装当前 SSL 连接对象的证书
	 * @param ssl {void*} SSL 连接对象，为 ssl_context 类型
	 * @return {bool} 配置 SSL 对象是否成功
	 */
	bool setup_certs(void* ssl);

public:
	/**
	 * 必须首先调用此函数设置 libmbedtls_all.so 的全路径
	 * @param path {const char*} libmbedtls_all.so 的全路径
	 */
	static void set_libpath(const char* libmbedtls);

	/**
	 * 可以显式调用本方法，动态加载 polarssl 动态库
	 */
	static void load(void);

public:
	// @override sslbase_conf
	sslbase_io* open(bool server_side, bool nblock);

private:
	friend class mbedtls_io;

	bool has_inited_;
	thread_mutex lock_;

	bool server_side_;

	void* conf_;
	void* entropy_;
	void* rnd_;
	void* cacert_;
	void* pkey_;
	void* cert_chain_;
	void* cache_;
	mbedtls_verify_t verify_mode_;

private:
	bool init_once(void);
	bool init_rand(void);
	void free_ca(void);
};

} // namespace acl
