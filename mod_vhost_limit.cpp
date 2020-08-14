/*
 *
 * Author           : Ivan Barrera (Bruce) 
 * Updated          : 2020.08.09
 * Status           : WIP
 * Current Version  : v0.3
 *
 * NAME
 *     mod_vhost_limit
 *
 * PURPOSE
 *       * Restrict the number of simultaneous connections per vhost
 *
 * HISTORY
 *     2020.08.09
 *       Rewriting most of the module now using C++ (or starting to)
 *
 */

#include <string>
#include "httpd.h"
#include "http_config.h"
#include "util_filter.h"
#include "apr_atomic.h"
#include "http_connection.h"
#include "http_log.h"
#include "apr-1/apr_strings.h"
#include "shm.h"

#define VERSION             0.3
#define MOD_NAME            "mod_vhost_limit"

extern "C" 
{
  APLOG_USE_MODULE(vhost_limit);

  void register_hooks(apr_pool_t* p);
  void *create_vhost_limit_config(apr_pool_t *p, server_rec *s);
  static const char *max_vhost_clients_set(cmd_parms *cmd, void *dummy, const char *arg);

  static const command_rec vhost_limit_cmds[] =
  {
      AP_INIT_TAKE1( "MaxVhostClients", (cmd_func) max_vhost_clients_set, NULL, RSRC_CONF, "'MaxVhostClients' followed by non-negative integer"),
      {NULL}
  };

  module AP_MODULE_DECLARE_DATA vhost_limit_module = {
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    create_vhost_limit_config, 
    NULL,
    vhost_limit_cmds,
	register_hooks
  };
};

typedef struct vhost_limit_conf_t {
    apr_uint32_t sid;
    int max_clients;
    int mode = 0;
	char *name;
} vhost_limit_conf;

typedef struct vh_data_t
{
  apr_uint32_t counter;
  volatile apr_uint32_t lock;
} vh_data;

apr_uint32_t global_sid = 0;
c_shm_pool *shm_pool;
vh_data *data;

static const char *max_vhost_clients_set(cmd_parms *cmd, void *dummy, const char *arg)
{
    int val;
    vhost_limit_conf *conf;

    val = atoi(arg);
    conf = (vhost_limit_conf *) ap_get_module_config(cmd->server->module_config, &vhost_limit_module);

    if (val < 1)
	{
        return "MaxVhostClients must be an integer greater than 0";
    }
	else 
	{
        conf->max_clients = val;
        conf->mode = 1;
        ap_log_error(APLOG_MARK,APLOG_INFO|APLOG_NOERRNO, 0 ,NULL, "%s: %s (%d) set MaxVhostClients = %d",MOD_NAME,cmd->server->server_hostname, conf->sid, val);
    }

    return NULL;
}

void *create_vhost_limit_config(apr_pool_t *p, server_rec *s)
{
  vhost_limit_conf *conf = (vhost_limit_conf *) apr_pcalloc(p, sizeof(vhost_limit_conf));
  char *path = s->server_hostname;

  if (path == NULL) return NULL;

  conf->max_clients = -1;
  conf->mode = 0;
  conf->name = (char *) apr_pstrdup(p, path);
  conf->sid = global_sid++;

  data = (vh_data *) shm_pool->base;
  data = data + conf->sid - 1;
  data->counter = 0;

  ap_log_error(APLOG_MARK,APLOG_DEBUG|APLOG_NOERRNO, 0 ,NULL, "%s: %s (%d) create config for %s",MOD_NAME, conf->name, conf->sid, path);

  return conf;
}

static int handle_r(request_rec *r)
{
  if (r->main) return DECLINED;

  vhost_limit_conf *conf = (vhost_limit_conf *) ap_get_module_config(r->server->module_config, &vhost_limit_module);
  apr_pool_t *pool = r->pool;

  data = (vh_data *) shm_pool->base;
  data = data + conf->sid;

  if (data->counter >= conf->max_clients)
  {
    ap_log_error(APLOG_MARK, APLOG_INFO, 0,NULL,"%s: Access to %s deferred, Max Clients %d exceeded (%d currently)",MOD_NAME, conf->name, conf->max_clients, data->counter);
    return HTTP_SERVICE_UNAVAILABLE;
  }

  char *s = (char*) apr_pcalloc(pool, sizeof(char)*8);
  apr_snprintf(s,8,"%d",conf->sid);
  apr_table_t *table = r->connection->notes;
  apr_table_setn(table,"sid",  s );

  apr_atomic_add32(&data->counter,1);

  ap_log_error(APLOG_MARK,APLOG_DEBUG|APLOG_NOERRNO, 0 ,NULL, "%s: handling IN %s (%s) = %d", MOD_NAME, conf->name, s, data->counter);

  return DECLINED;
}

static int pre_close(conn_rec *c) 
{
  const char *tid = apr_table_get( c->notes , "sid" );

  if (tid == NULL) return OK;

  apr_uint32_t sid = atoi( tid );

  data = (vh_data *) shm_pool->base;
  data = data + sid;

  apr_atomic_dec32(&data->counter);

  ap_log_error(APLOG_MARK,APLOG_DEBUG|APLOG_NOERRNO, 0 ,NULL, "%s: close OUT (%d) = %d", MOD_NAME, sid, data->counter);

  return OK;
}

void register_hooks(apr_pool_t* p)
{
  shm_pool = new c_shm_pool(p, (apr_size_t) sizeof(vh_data) * 100 );
  ap_hook_handler(handle_r, NULL, NULL, APR_HOOK_REALLY_FIRST);

  ap_hook_pre_close_connection(pre_close, NULL, NULL, APR_HOOK_REALLY_FIRST);
}

