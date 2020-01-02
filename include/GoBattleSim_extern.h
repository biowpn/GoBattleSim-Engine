
/**
 * This file defines the C-interface for GoBattleSim.
 */

#ifndef _GOBATTLESIM_EXTERN_H_
#define _GOBATTLESIM_EXTERN_H_

#ifdef __cplusplus
extern "C"
{
#endif

	/**
     * initialize new simulation. This will clear all output.
	 * 
	 * @param input_j input in JSON
     */
	void GBS_prepare(const char *input_j);

	/**
     * run the new simulation configured by the latest GBS_prepare().
     */
	void GBS_run();

	/**
     * collect simulation output produced by the lastest GBS_run().
	 * 
	 * @param output_j if not NULL, will be copied into the output JSON.
	 * @param output_j_len if not NULL, will be set to the length of the above-mentioned JSON.
     */
	void GBS_collect(char *output_j, int *output_j_len);

	/**
	 * set game master parameters.
	 * 
	 * @param gm_j GBS-format game master in JSON
	 */
	void GBS_GameMaster_set(const char *gm_j);

	/**
	 * get game master parameters.
	 * 
	 * @param gm_j if not NULL, will be copied into the GBS game master in JSON.
	 * @param gm_j_len if not NULL, will be set to the length of the above-mentioned JSON.
	 */
	void GBS_GameMaster_get(char *gm_j, int *gm_j_len);

#ifdef __cplusplus
}
#endif

#endif
