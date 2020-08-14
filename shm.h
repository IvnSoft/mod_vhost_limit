
#include "apr-1/apr_shm.h"

class c_shm_pool
{

  public:
    void *base;
	int size = 0;

    c_shm_pool(apr_pool_t *p, apr_size_t shm_size);
    ~c_shm_pool();

  c_shm_pool *next;

};

