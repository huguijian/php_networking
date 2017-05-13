/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_networking.h"
#include "networking_server.h"

/*socket include files*/

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#define BUFFLEN 1024
#define BACKLOG 5

zend_class_entry *networking_class_ce;


static PHP_METHOD(networking,__construct);
static PHP_METHOD(networking, on);
static PHP_METHOD(networking,send);
static PHP_METHOD(networking,run);


ZEND_BEGIN_ARG_INFO_EX(arginfo_networking_on, 0, 0, 2)
    ZEND_ARG_INFO(0, cb_name)
    ZEND_ARG_INFO(0, zcallback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_networking_send, 0, 0, 2)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

struct dataPacket{
    unsigned int msgLen;
    char data[1024];
};

ssize_t send_other(int fd, void *buf, size_t count)
{
        int left = count ;
        char * ptr = (char *)buf;
        while(left >0)
        {
                int writeBytes = send(fd,ptr,left,0);
                if(writeBytes<0)
                {
                        if(errno == EINTR)
                                continue;
                        return -1;
                }
                else if(writeBytes == 0)
                        continue;
                left -= writeBytes;
                ptr  += writeBytes;
        }
        return left;
}

ssize_t recv_other(int fd,void *buf,size_t count)
{
    int left = count ; //剩下的字节
    char *ptr = (char*)buf ;
    while(left>0)
    {
            int readBytes = recv(fd,ptr,left,0);
            if(readBytes< 0)//read函数小于0有两种情况：1中断 2出错
            {
                    if(errno == EINTR)//读被中断
                    {
                            continue;
                    }

                    return -1;
            }
            if(readBytes == 0)//读到了EOF
            {

                return count-left;
            }
            left -= readBytes;
            ptr  += readBytes;
    }

    return left;
}

static int handle_request(int s_c,zval *obj)
{

    time_t now;

    int msg_len = 0;

    struct dataPacket readPacket;
    memset(&readPacket,0,sizeof(readPacket));
    msg_len = recv_other(s_c,&readPacket.msgLen,4);

    int dataBytes = ntohl(readPacket.msgLen); //字节序的转换

    int readBytes = recv_other(s_c,&readPacket.data,dataBytes); //读取出后续的数据

    if(readBytes==0) {

        zval *func_on_message;
        zval rv;
        zval retval;
        zval params[2];

        uint param_count;
        param_count = sizeof(params)/sizeof(zval);



        zval temp_data;
        array_init(&temp_data);

        add_assoc_long_ex(&temp_data, ZEND_STRL("fd"), s_c);
        add_assoc_stringl_ex(&temp_data,ZEND_STRL("data"),ZEND_STRL(readPacket.data));
        convert_to_object_ex(&temp_data);

        params[0] = (*obj);
        params[1] = temp_data;

        func_on_message = zend_read_property(networking_class_ce, obj, ZEND_STRL("onMessage"), 1 TSRMLS_CC,&rv);
        if(!ZVAL_IS_NULL(func_on_message)) {
            call_user_function(EG(table_function),NULL,func_on_message,&retval,param_count,params);
        }

        memset(&readPacket,0,sizeof(readPacket));
        zval_ptr_dtor(&retval);
        zval_ptr_dtor(&params[1]);

    }
    close(s_c);
}


static int handle_connect(int s_s,zval *obj)
{
    int s_c;
    struct sockaddr_in from;
    socklen_t len = sizeof(from);
    zval *func_on_connect;
    zval rv;
    zval retval;
    while(1)
    {
       s_c = accept(s_s,(struct sockaddr*)&from,&len);
       if(s_c>0){

           if(fork()>0) {
               close(s_c);
           }else{

               func_on_connect = zend_read_property(networking_class_ce,obj,ZEND_STRL("onConnect"),1 TSRMLS_CC,&rv);
               if(!ZVAL_IS_NULL(func_on_connect)) {
                    call_user_function(EG(table_function),NULL,func_on_connect,&retval,0,NULL);
               }

               handle_request(s_c,obj);
               return 0;
           }
       }
    }

}

static PHP_METHOD(networking,__construct)
{
    char *server_type,*host;
    long port;
    long server_type_len,host_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &server_type,&server_type_len,&host,&host_len,&port) == FAILURE) {
        php_error_docref(NULL, E_WARNING, "Invalid parameter");
        return;
    }
    zend_update_property_stringl(networking_class_ce, getThis(), ZEND_STRL("server_type"), server_type, server_type_len TSRMLS_CC);
    zend_update_property_stringl(networking_class_ce, getThis(), ZEND_STRL("host"),host,host_len TSRMLS_CC);
    zend_update_property_long(networking_class_ce,getThis(),ZEND_STRL("port"),port TSRMLS_CC);

}


static PHP_METHOD(networking,send)
{
    struct dataPacket writePacket;
    long fd;
    char *data;
    long data_len;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"ls",&fd,&data,&data_len)==FAILURE)
    {
        return;
    }

    int n = strlen(data);//计算要发送的数据的字节数
    writePacket.msgLen = htonl(n);    //将该字节数保存在msgLen字段，注意字节序的转换
    strcpy(writePacket.data,data);
    send_other(fd,&writePacket,4+n);   //发送数据，数据长度为4个字节的msgLen 加上data长度
    memset(&writePacket,0,sizeof(writePacket));
}

static PHP_METHOD(networking,on)
{
    char *cb_name;
    long cb_name_len;
    zval *callback_func;

    char *cb_arr[2] = {
        "onConnect",
        "onMessage"
    };

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &cb_name, &cb_name_len, &callback_func) == FAILURE)
    {
        return;
    }
    /*
    for(i=0;i<2;i++) {
        if(strcmp(cb_name,cb_arr[i])==0) {
            zend_update_property(networking_class_ce, getThis(), ZEND_STRL(cb_arr[i]), callback_func TSRMLS_CC);
        }
    }
    **/
    if(strcmp(cb_name,"onConnect")==0) {

        zend_update_property(networking_class_ce, getThis(), ZEND_STRL("onConnect"), callback_func TSRMLS_CC);
    }else if(strcmp(cb_name,"onMessage")==0){

        zend_update_property(networking_class_ce, getThis(), ZEND_STRL("onMessage"), callback_func TSRMLS_CC);
    }

   RETURN_TRUE;

}



static PHP_METHOD(networking,run)
{
    int s_s;

    zval *port,p_rv;
    zval *host,h_rv;

    int val;
    port = zend_read_property(networking_class_ce, getThis(), ZEND_STRL("port"), 0 TSRMLS_CC, &p_rv);
    host = zend_read_property(networking_class_ce, getThis(), ZEND_STRL("host"), 0 TSRMLS_CC, &h_rv);
    struct sockaddr_in local;
    s_s = socket(AF_INET,SOCK_STREAM,0);
    memset(&local,0,sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = inet_addr(Z_STRVAL_P(host));
    local.sin_port = htons(Z_LVAL_P(port));

    setsockopt(s_s, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    bind(s_s,(struct sockaddr*)&local,sizeof(local));
    listen(s_s,BACKLOG);

    handle_connect(s_s,getThis());

    close(s_s);
}

const zend_function_entry networking_methods[] = {
    PHP_ME(networking, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(networking, send, arginfo_networking_send, ZEND_ACC_PUBLIC)
    PHP_ME(networking, on, arginfo_networking_on, ZEND_ACC_PUBLIC)
    PHP_ME(networking, run, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

NETWORKING_STARTUP_FUNCTION(server) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "networking", networking_methods);
    networking_class_ce = zend_register_internal_class(&ce);
    return SUCCESS;
}




