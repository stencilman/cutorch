#include "THCStorage.h"
#include "THCGeneral.h"

void THCudaStorage_set(THCState *state, THCudaStorage *self, long index, float value)
{
  THArgCheck((index >= 0) && (index < self->size), 2, "index out of bounds");
  THCudaCheck(cudaMemcpy(self->data + index, &value, sizeof(float), cudaMemcpyHostToDevice));
}

float THCudaStorage_get(THCState *state, const THCudaStorage *self, long index)
{
  float value;
  THArgCheck((index >= 0) && (index < self->size), 2, "index out of bounds");
  THCudaCheck(cudaMemcpy(&value, self->data + index, sizeof(float), cudaMemcpyDeviceToHost));
  return value;
}

THCudaStorage* THCudaStorage_new(THCState *state)
{
  THCudaStorage *storage = (THCudaStorage*)THAlloc(sizeof(THCudaStorage));
  storage->data = NULL;
  storage->size = 0;
  storage->refcount = 1;
  storage->flag = TH_STORAGE_REFCOUNTED | TH_STORAGE_RESIZABLE | TH_STORAGE_FREEMEM;
  return storage;
}

THCudaStorage* THCudaStorage_newWithSize(THCState *state, long size)
{
  THArgCheck(size >= 0, 2, "invalid size");

  if(size > 0)
  {
    THCudaStorage *storage = (THCudaStorage*)THAlloc(sizeof(THCudaStorage));
    THCudaCheck(cudaMalloc((void**)&(storage->data), size * sizeof(float)));
    storage->size = size;
    storage->refcount = 1;
    storage->flag = TH_STORAGE_REFCOUNTED | TH_STORAGE_RESIZABLE | TH_STORAGE_FREEMEM;
    return storage;
  }
  else
  {
    return THCudaStorage_new(state);
  }
}

THCudaStorage* THCudaStorage_newWithSize1(THCState *state, float data0)
{
  THCudaStorage *self = THCudaStorage_newWithSize(state, 1);
  THCudaStorage_set(state, self, 0, data0);
  return self;
}

THCudaStorage* THCudaStorage_newWithSize2(THCState *state, float data0, float data1)
{
  THCudaStorage *self = THCudaStorage_newWithSize(state, 2);
  THCudaStorage_set(state, self, 0, data0);
  THCudaStorage_set(state, self, 1, data1);
  return self;
}

THCudaStorage* THCudaStorage_newWithSize3(THCState *state, float data0, float data1, float data2)
{
  THCudaStorage *self = THCudaStorage_newWithSize(state, 3);
  THCudaStorage_set(state, self, 0, data0);
  THCudaStorage_set(state, self, 1, data1);
  THCudaStorage_set(state, self, 2, data2);
  return self;
}

THCudaStorage* THCudaStorage_newWithSize4(THCState *state, float data0, float data1, float data2, float data3)
{
  THCudaStorage *self = THCudaStorage_newWithSize(state, 4);
  THCudaStorage_set(state, self, 0, data0);
  THCudaStorage_set(state, self, 1, data1);
  THCudaStorage_set(state, self, 2, data2);
  THCudaStorage_set(state, self, 3, data3);
  return self;
}

THCudaStorage* THCudaStorage_newWithMapping(THCState *state, const char *fileName, long size, int isShared)
{
  THError("not available yet for THCudaStorage");
  return NULL;
}

THCudaStorage* THCudaStorage_newWithData(THCState *state, float *data, long size)
{
  THCudaStorage *storage = (THCudaStorage*)THAlloc(sizeof(THCudaStorage));
  storage->data = data;
  storage->size = size;
  storage->refcount = 1;
  storage->flag = TH_STORAGE_REFCOUNTED | TH_STORAGE_RESIZABLE | TH_STORAGE_FREEMEM;
  return storage;
}

void THCudaStorage_retain(THCState *state, THCudaStorage *self)
{
  if(self && (self->flag & TH_STORAGE_REFCOUNTED))
    ++self->refcount;
}

void THCudaStorage_free(THCState *state, THCudaStorage *self)
{
  if(!(self->flag & TH_STORAGE_REFCOUNTED))
    return;

  if (--(self->refcount) <= 0)
  {
    if(self->flag & TH_STORAGE_FREEMEM)
      THCudaCheck(cudaFree(self->data));
    THFree(self);
  }
}
