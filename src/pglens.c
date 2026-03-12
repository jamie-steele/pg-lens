#include "postgres.h"

#include "executor/spi.h"
#include "fmgr.h"
#include "utils/builtins.h"

#include "pglens.h"

PG_MODULE_MAGIC;

/*
 * The extension entrypoints stay intentionally small. SQL-callable wrappers
 * only handle argument conversion, SPI lifecycle, and delegation.
 */

PG_FUNCTION_INFO_V1(pglens_register_lens);
PG_FUNCTION_INFO_V1(pglens_recompute_lens);

static void
pglens_spi_connect(void)
{
	int rc = SPI_connect();

	if (rc != SPI_OK_CONNECT)
		elog(ERROR, "pg-lens could not connect to SPI");
}

static void
pglens_spi_finish(void)
{
	int rc = SPI_finish();

	if (rc != SPI_OK_FINISH)
		elog(ERROR, "pg-lens could not finish SPI");
}

Datum
pglens_register_lens(PG_FUNCTION_ARGS)
{
	char   *lens_name = text_to_cstring(PG_GETARG_TEXT_PP(0));
	Oid		view_oid = PG_GETARG_OID(1);
	char   *target_schema = text_to_cstring(PG_GETARG_TEXT_PP(2));
	char   *target_name = text_to_cstring(PG_GETARG_TEXT_PP(3));
	char   *key_column = text_to_cstring(PG_GETARG_TEXT_PP(4));

	pglens_spi_connect();

	PG_TRY();
	{
		pglens_register_lens_internal(lens_name,
									  view_oid,
									  target_schema,
									  target_name,
									  key_column);
		pglens_spi_finish();
	}
	PG_CATCH();
	{
		pglens_spi_finish();
		PG_RE_THROW();
	}
	PG_END_TRY();

	PG_RETURN_VOID();
}

Datum
pglens_recompute_lens(PG_FUNCTION_ARGS)
{
	char   *lens_name = text_to_cstring(PG_GETARG_TEXT_PP(0));
	char   *key_text = text_to_cstring(PG_GETARG_TEXT_PP(1));
	bool	present_after_recompute;

	pglens_spi_connect();

	PG_TRY();
	{
		present_after_recompute = pglens_recompute_lens_internal(lens_name,
																 key_text);
		pglens_spi_finish();
	}
	PG_CATCH();
	{
		pglens_spi_finish();
		PG_RE_THROW();
	}
	PG_END_TRY();

	PG_RETURN_BOOL(present_after_recompute);
}
