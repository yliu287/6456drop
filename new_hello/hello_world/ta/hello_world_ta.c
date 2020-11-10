/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <hello_world_ta.h>
/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");

	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	IMSG("Hello World!\n");

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Goodbye!\n");
}

static TEE_Result inc_value(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	//intialize secure storage
	TEE_ObjectHandle object;
	TEE_Result res;
	char obj_id[]="object#1";
	size_t obj_id_sz=8;
	uint32_t obj_data_flag;
	int *data;
	size_t data_sz=4;
	uint32_t read_bytes;
	TEE_ObjectInfo object_info;
	obj_data_flag = TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_WRITE_META | TEE_DATA_FLAG_OVERWRITE;		/* destroy existing object of same ID */
	
	data = TEE_Malloc(data_sz, 0);
	//storage initalize over
	DMSG("now has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;
	DMSG("reading params");
	//reading
	if (params[1].value.a==1){
        res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
								obj_id, obj_id_sz,
													TEE_DATA_FLAG_ACCESS_READ |
																		TEE_DATA_FLAG_SHARE_READ,
																							&object);
	if (res != TEE_SUCCESS) {
				EMSG("Failed to open persistent object, res=0x%08x", res);
						TEE_Free(obj_id);
								TEE_Free(data);
										return res;
											}

		res = TEE_GetObjectInfo1(object, &object_info);
			if (res != TEE_SUCCESS) {
						EMSG("Failed to create persistent object, res=0x%08x", res);
									}
	
	res = TEE_ReadObjectData(object, data, object_info.dataSize,
							 &read_bytes);
		if (res == TEE_SUCCESS)
					TEE_MemMove(&(params[0].value.a), data, read_bytes);
	}//
	DMSG("startgetting");
	IMSG("Got value: %u from NW", params[0].value.a);
	params[0].value.a+=2;
	if (params[0].value.a>100)
		return TEE_ERROR_EXCESS_DATA;
	IMSG("Increase value to: %u", params[0].value.a);
	DMSG("startsaving");
	//saving 
	TEE_MemMove(data, &(params[0].value.a), data_sz);

	DMSG("copy before create");
res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,obj_id, obj_id_sz,obj_data_flag,TEE_HANDLE_NULL,NULL,0,&object);
	if (res==TEE_ERROR_ACCESS_CONFLICT){
		DMSG("conflict error not an issue try open");
		res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,obj_id,obj_id_sz,TEE_DATA_FLAG_ACCESS_READ |
									TEE_DATA_FLAG_ACCESS_WRITE,&object);
		TEE_CloseObject(object);
		TEE_Free(data);
		return TEE_SUCCESS;
	}
	//if (res != TEE_SUCCESS) {
	//			EMSG("TEE_CreatePersistentObject failed 0x%08x", res);
	//					TEE_Free(obj_id);
	//							TEE_Free(data);
	//									return res;
        //											}
	DMSG("successfully creating");
	res = TEE_WriteObjectData(object, data, data_sz);
		if (res != TEE_SUCCESS) {
					DMSG("Something wrong");
					EMSG("TEE_WriteObjectData failed 0x%08x", res);
							TEE_CloseAndDeletePersistentObject1(object);
								} else {
									DMSG("before closing");		
									TEE_CloseObject(object);
												}
		DMSG("successfully writing");	
		//TEE_Free(obj_id);
				TEE_Free(data);
	//
	DMSG("successfully free before return");
	return TEE_SUCCESS;
}

static TEE_Result dec_value(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	IMSG("Got value: %u from NW", params[0].value.a);
	params[0].value.a--;
	if (params[0].value.a<0)
		return TEE_ERROR_EXCESS_DATA;
	IMSG("Decrease value to: %u", params[0].value.a);

	return TEE_SUCCESS;
}
/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {
	case TA_HELLO_WORLD_CMD_INC_VALUE:
		return inc_value(param_types, params);
	case TA_HELLO_WORLD_CMD_DEC_VALUE:
		return dec_value(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
