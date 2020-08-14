
#include "httpd.h"
#include "http_log.h"
#include "apr-1/apr_shm.h"
#include "shm.h"

APLOG_USE_MODULE(vhost_limit);

#define VERSION             0.3
#define MOD_NAME            "mod_vhost_limit"

c_shm_pool::c_shm_pool(apr_pool_t *p, apr_size_t shm_size)
{
  apr_shm_t *shm;
  apr_size_t retsize;
  apr_status_t status;
  int t;

  status = apr_shm_create(&shm, shm_size, NULL, p);
  if (status != APR_SUCCESS) 
  {
      ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL, "%s: Error creating shm block",MOD_NAME);
      return; 
  }
  retsize = apr_shm_size_get(shm);
  if (retsize != shm_size) 
  {
      ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL, "%s: Error allocating shared memory block",MOD_NAME);
      return;
  }

  base = apr_shm_baseaddr_get(shm);
  if (base == NULL) 
  {
      ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL, "%s: Error creating status block",MOD_NAME);
      return;
  }
  memset(base, 0, retsize);
	  
  ap_log_error(APLOG_MARK, APLOG_NOERRNO|APLOG_NOTICE, 0, NULL, "%s: Created SHM block at 0x%pp , size %d",MOD_NAME, base, retsize);

}

c_shm_pool::~c_shm_pool() 
{


}

