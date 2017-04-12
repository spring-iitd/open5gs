#define TRACE_MODULE _esm_sm

#include "core_debug.h"

#include "nas_message.h"

#include "mme_event.h"
#include "esm_handler.h"
#include "esm_build.h"

void esm_state_initial(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    mme_sm_trace(1, e);

    FSM_TRAN(s, &esm_state_operational);
}

void esm_state_final(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    mme_sm_trace(1, e);
}

void esm_state_operational(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(1, e);

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case MME_EVT_ESM_LO_INFO_REQ:
        case MME_EVT_ESM_LO_CREATE_SESSION:
        {
            index_t index = event_get_param1(e);
            mme_esm_t *esm = NULL;
            mme_ue_t *ue = NULL;

            d_assert(index, return, "Null param");
            esm = mme_esm_find(index);
            d_assert(esm, return, "Null param");
            ue = esm->ue;
            d_assert(ue, return, "Null param");

            switch(event_get(e))
            {
                case MME_EVT_ESM_LO_INFO_REQ:
                {
                    pkbuf_t *pkbuf = NULL;
                    status_t rv;

                    rv = esm_build_information_request(&pkbuf, esm);
                    d_assert(rv == CORE_OK, break, "esm_build failed");

                    mme_event_nas_to_s1ap(ue, pkbuf);
                    d_info("[NAS] ESM information request : "
                            "UE[%s] <--- ESM[%d]", ue->imsi_bcd, esm->pti);
                    break;
                }
                case MME_EVT_ESM_LO_CREATE_SESSION:
                {
#if 0
                    pkbuf_t *pkbuf = NULL;
                    status_t rv;

                    rv = esm_build_information_request(&pkbuf, esm);
                    d_assert(rv == CORE_OK, break, "esm_build failed");

                    mme_event_nas_to_s1ap(ue, pkbuf);
                    d_info("[NAS] ESM information request : "
                            "UE[%s] <--- ESM[%d]", ue->imsi_bcd, esm->pti);
#endif
                    break;
                }
            }

            break;
        }
        case EVT_MSG_MME_ESM:
        {
            index_t index = event_get_param1(e);
            mme_esm_t *esm = NULL;
            mme_ue_t *ue = NULL;
            nas_message_t *message = NULL;

            d_assert(index, return, "Null param");
            esm = mme_esm_find(index);
            d_assert(esm, return, "Null param");
            ue = esm->ue;
            d_assert(ue, return, "Null param");
            message = (nas_message_t *)event_get_param3(e);
            d_assert(message, break, "Null param");

            switch(message->esm.h.message_type)
            {
                case NAS_PDN_CONNECTIVITY_REQUEST:
                {
                    esm_handle_pdn_connectivity_request(
                            esm, &message->esm.pdn_connectivity_request);
                    d_info("[NAS] PDN connectivity request : "
                            "UE[%s] --> ESM[%d]", ue->imsi_bcd, esm->pti);
                    break;
                }
                case NAS_ESM_INFORMATION_RESPONSE:
                {
                    /* FIXME : SGW Selection */
                    esm->sgw = mme_sgw_first();

                    esm_handle_information_response(
                            esm, &message->esm.esm_information_response);
                    d_info("[NAS] ESM information response : "
                            "UE[%s] --> ESM[%d]", ue->imsi_bcd, esm->pti);
                    break;
                }
                default:
                {
                    d_warn("Not implemented(type:%d)", 
                            message->esm.h.message_type);
                    break;
                }
            }
            break;
        }

        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}

void esm_state_exception(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(1, e);

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}
