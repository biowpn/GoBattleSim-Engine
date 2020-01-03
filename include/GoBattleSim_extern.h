
/**
 * This file defines the C-interface for GoBattleSim.
 */

#ifndef _GOBATTLESIM_EXTERN_H_
#define _GOBATTLESIM_EXTERN_H_

#ifdef __EMSCRIPTEN__
// Forces LLVM to not dead-code-eliminate a function.
#include <emscripten.h>
#define FUNCTION_PREFIX EMSCRIPTEN_KEEPALIVE
#else
#define FUNCTION_PREFIX
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * return the version string in the format "{major}.{minor}.{patch}".
	 */
	const char *FUNCTION_PREFIX GBS_version();

	/**
	 * get the last error message.
	 */
	const char *FUNCTION_PREFIX GBS_error();

	/**
     * initialize new simulation. This will clear all output.
	 * 
	 * @param input_j simulation input in JSON
     */
	void FUNCTION_PREFIX GBS_prepare(const char *input_j);

	/**
     * run the new simulation configured by the latest GBS_prepare().
     */
	void FUNCTION_PREFIX GBS_run();

	/**
     * collect simulation output produced by the lastest GBS_run().
	 * 
	 * @return simulation output in JSON
     */
	const char *FUNCTION_PREFIX GBS_collect();

	/**
	 * if @param gm_j is not NULL, set GBS game master parameters by it.
	 * 
	 * @return GBS-format game master in JSON
	 */
	const char *FUNCTION_PREFIX GBS_config(const char *gm_j);

#ifdef __cplusplus
}
#endif

#endif
