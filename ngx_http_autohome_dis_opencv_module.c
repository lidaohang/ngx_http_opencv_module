#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <sys/types.h>
#include <image_file_block.h>

/* Module config */
typedef struct {
	ngx_str_t ed;
} ngx_http_autohome_dis_opencv_loc_conf_t;

extern void nginx_module_invoke_opencv_resize(empty_struct * p,image_file_t * retval);
empty_struct p;

static char *ngx_http_autohome_dis_opencv(ngx_conf_t *cf, ngx_command_t *cmd,void *conf);
static void *ngx_http_autohome_dis_opencv_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_autohome_dis_opencv_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);


/* Directives */
static ngx_command_t ngx_http_autohome_dis_opencv_commands[] = { {
		ngx_string("autohome_dis_opencv"), NGX_HTTP_LOC_CONF
		| NGX_CONF_NOARGS, ngx_http_autohome_dis_opencv, NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_autohome_dis_opencv_loc_conf_t, ed), NULL },

ngx_null_command };

/* Http context of the module */
static ngx_http_module_t ngx_http_autohome_dis_opencv_module_ctx = { NULL, /* preconfiguration */
NULL, /* postconfiguration  (在创建和读取该模块的配置信息之后被调用)*/

NULL, /* create main configuration */
NULL, /* init main configuration */

NULL, /* create server configuration */
NULL, /* merge server configuration */

ngx_http_autohome_dis_opencv_create_loc_conf, /* create location configration */
ngx_http_autohome_dis_opencv_merge_loc_conf /* merge location configration */
};

/* Module */
ngx_module_t ngx_http_autohome_dis_opencv_module = { NGX_MODULE_V1,
		&ngx_http_autohome_dis_opencv_module_ctx, /* module context */
		ngx_http_autohome_dis_opencv_commands, /* module directives */
		NGX_HTTP_MODULE, /* module type */
		NULL, /* init master */
		NULL, /* init module */
		NULL, /* init process */
		NULL, /* init thread */
		NULL, /* exit thread */
		NULL, /* exit process */
		NULL, /* exit master */
		NGX_MODULE_V1_PADDING };

static char* ngx_str_replace(char *search, char *replace, char *subject) {
	char *p = NULL, *old = NULL, *new_subject = NULL;
	int c = 0, search_size;

	search_size = strlen(search);

	//Count how many occurences
	for (p = strstr(subject, search); p != NULL ;
			p = strstr(p + search_size, search)) {
		c++;
	}

	//Final size
	c = (strlen(replace) - search_size) * c + strlen(subject);

	//New subject with new size
	new_subject = malloc(c);
	memset(new_subject,0,sizeof(char)*c);
	//Set it to blank
	strcpy(new_subject, "");

	//The start position
	old = subject;

	for (p = strstr(subject, search); p != NULL ;
			p = strstr(p + search_size, search)) {
		//move ahead and copy some text from original subject , from a certain position
		strncpy(new_subject + strlen(new_subject), old, p - old);

		//move ahead and copy the replacement text
		strcpy(new_subject + strlen(new_subject), replace);

		//The new start position after this search match
		old = p + search_size;
	}

	//Copy the part after the last search match
	strcpy(new_subject + strlen(new_subject), old);

	return new_subject;
}

static char *ngx_str_copy(const char *str) {
	int len = strlen(str) + 1;
	char *buf = malloc(len);
	if (NULL == buf)
		return NULL ;
	memcpy(buf, str, len);
	return buf;
}

static int ngx_str_split(const char *str, char *parts[], const char *delimiter) {
	char *pch;
	int i = 0;
	char *tmp = ngx_str_copy(str);
	pch = strtok(tmp, delimiter);

	parts[i++] = ngx_str_copy(pch);

	while (pch) {
		pch = strtok(NULL, delimiter);
		if (NULL == pch)
			break;
		parts[i++] = ngx_str_copy(pch);
	}

	free(tmp);
	free(pch);
	return i;
}

static image_file_t * ngx_get_image(ngx_http_request_t *r) {

	image_file_t *images;
	images = malloc(sizeof(image_file_t));

	//get uri
	ngx_str_t uri = r->unparsed_uri;
	u_char* ngx_string_uri;

	ngx_string_uri = ngx_pcalloc(r->pool, (uri.len + 1) * sizeof(u_char));
	///ngx_memset(ngx_string_uri, 0, sizeof(u_char) * (uri.len + 1));
	ngx_sprintf(ngx_string_uri, "%V", &uri);

	//split uri  /data/logo/80x60_30_1_logo.jpg
	char **ngx_array_uri = ngx_pcalloc(r->pool,(uri.len + 1) * sizeof(char));
	size_t uri_size = ngx_str_split((const char *) ngx_string_uri, ngx_array_uri,
			"/");

	//split filename  80x60_30_1_logo.jpg
	char **ngx_array_image = ngx_pcalloc(r->pool,(uri.len+1) * sizeof(char));
	size_t image_size = ngx_str_split(ngx_array_uri[uri_size - 1], ngx_array_image,
			"_");

	char* str_image_resize;
	char* str_image_quality;
	char* str_image_watermark;
	char* str_image;

	if (image_size >= 2) {
		char ** ngx_array_image_size = ngx_pcalloc(r->pool,(uri.len+1) * sizeof(char));
		size_t image_size = ngx_str_split(ngx_array_image[0], ngx_array_image_size,
				"x");

		// width hegith
		if (image_size == 2) {
			images->width = atoi(ngx_array_image_size[0]);
			images->height = atoi(ngx_array_image_size[1]);
		}

		str_image_resize = ngx_pcalloc(r->pool, sizeof(char) * (strlen(ngx_array_image[0])+2));
		strcpy(str_image_resize, ngx_array_image[0]);
		strcat(str_image_resize, "_");
	}
	if (image_size >= 3) {
		//quality
		images->quality = atof(ngx_array_image[1]);
		str_image_quality = ngx_pcalloc(r->pool, sizeof(char) * (strlen(ngx_array_image[1])+2));
		strcpy(str_image_quality, ngx_array_image[1]);
		strcat(str_image_quality, "_");
	}
	if (image_size > 3) {
		images->watermark = atoi(ngx_array_image[2]);

		str_image_watermark = ngx_pcalloc(r->pool, sizeof(char) * (strlen(ngx_array_image[2])+2));
		strcpy(str_image_watermark, ngx_array_image[2]);
		strcat(str_image_watermark, "_");
	}
	if(str_image_resize != NULL && str_image_quality != NULL  && str_image_watermark == NULL){
		str_image = ngx_pcalloc(r->pool, sizeof(char) * (strlen(str_image_resize)+strlen(str_image_quality)+1));
		strcat(str_image, str_image_resize);
		strcat(str_image, str_image_quality);
	}else if(str_image_resize != NULL && str_image_quality != NULL && str_image_watermark != NULL){
		str_image = ngx_pcalloc(r->pool, sizeof(char) * (strlen(str_image_resize)+strlen(str_image_quality)+strlen(str_image_watermark)+1));
		strcat(str_image, str_image_resize);
		strcat(str_image, str_image_quality);
		strcat(str_image, str_image_watermark);
	}else{
		return NULL;
	}

	char* new_uri = ngx_str_replace(str_image, "", (char *)ngx_string_uri);
	images->path = ngx_pcalloc(r->pool,(strlen(new_uri) + 1) * sizeof(char));
	ngx_cpystrn((u_char *)images->path,(u_char *)new_uri,(strlen(new_uri) + 1)* sizeof(char));
	free(new_uri);
	return images;
}


/* Handler function */
static ngx_int_t ngx_http_autohome_dis_opencv_handler(ngx_http_request_t *r) {
	ngx_int_t rc;
	ngx_buf_t *b;
	ngx_chain_t out;

	ngx_http_autohome_dis_opencv_loc_conf_t *elcf;
	elcf = ngx_http_get_module_loc_conf(r, ngx_http_autohome_dis_opencv_module);

	if (!(r->method & (NGX_HTTP_HEAD | NGX_HTTP_GET | NGX_HTTP_POST))) {
		return NGX_HTTP_NOT_ALLOWED;
	}

	r->headers_out.content_type.len = sizeof("image/jpeg") - 1;
	r->headers_out.content_type.data = (u_char *) "image/jpeg";

	image_file_t* images = ngx_get_image(r);
	if(images == NULL){
		return NGX_HTTP_NOT_ALLOWED;
	}

	// process image begin

	nginx_module_invoke_opencv_resize(&p,images);
	if (images->code != NGX_HTTP_OK) {
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "");
	}
	if (images->len == 0 || images->data == NULL ) {
		if (images->code == NGX_HTTP_NOT_FOUND)//���UNC������ͼƬ��ֻ����404�ļ�������
		{
			ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
					"##########image data 's len = 0 or is NULL, statusCode=404\n");
			ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,images->path);
			return images->code;
		}
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,images->path);
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}


	r->headers_out.status = images->code;
	r->headers_out.content_length_n = images->len;		//��ʱͼƬ���ȱض�����0

	if (r->method == NGX_HTTP_HEAD) {
		rc = ngx_http_send_header(r);
		if (rc != NGX_OK) {
			return rc;
		}
	}

	b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
	if (b == NULL ) {
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
				"Failed to allocate response buffer.");
		free(images->data);
		free(images->path);
		free(images);
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}
	out.buf = b;
	out.next = NULL;

	int len = images->len;
	u_char * ngx_data = ngx_pcalloc(r->pool, len * sizeof(u_char));
	ngx_memcpy(ngx_data, images->data, len);

	free(images->data);			//���ͷ�opencv���
	free(images);

	b->pos = ngx_data;
	b->last = ngx_data + len;
	b->memory = 1;
	b->last_buf = 1;
	rc = ngx_http_send_header(r);
	if (rc != NGX_OK) {
		return rc;
	}

	return ngx_http_output_filter(r, &out);
}

static char *ngx_http_autohome_dis_opencv(ngx_conf_t *cf, ngx_command_t *cmd,
		void *conf) {
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module) ;
	clcf->handler = ngx_http_autohome_dis_opencv_handler;
	ngx_conf_set_str_slot(cf, cmd, conf);
	return NGX_CONF_OK ;
}

static void *ngx_http_autohome_dis_opencv_create_loc_conf(ngx_conf_t *cf) {
	ngx_http_autohome_dis_opencv_loc_conf_t *conf;
	conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_autohome_dis_opencv_loc_conf_t));
	if (conf == NULL ) {
		return NGX_CONF_ERROR ;
	}
	conf->ed.len = 0;     
	conf->ed.data = NULL;  
	return conf;
}

static char *ngx_http_autohome_dis_opencv_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child) {     
	ngx_http_autohome_dis_opencv_loc_conf_t *prev = parent;     
	ngx_http_autohome_dis_opencv_loc_conf_t *conf = child;       
	ngx_conf_merge_str_value(conf->ed, prev->ed, "");       
	return NGX_CONF_OK; 
} 
