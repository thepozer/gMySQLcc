#include "gTableUtils.h"

#define __VAR_ARFIELDTYPES__

extern p_fieldTypeCapability * arFieldTypes;
extern int szarFieldTypes;


/* Manage Field's infos */

p_fieldInfo createFieldInfo (int status) {
	p_fieldInfo p_fi;

	p_fi = (p_fieldInfo) g_try_malloc(sizeof(fieldInfo));
	if (p_fi != (p_fieldInfo)NULL) {
		p_fi->name = (GString*)NULL;
		p_fi->type_idx = -1;
		p_fi->length = 0;
		p_fi->set = (GString*)NULL;
		p_fi->def = (GString*)NULL;
		p_fi->unsign = FALSE;
		p_fi->zerofill = FALSE;
		p_fi->binary = FALSE;
		p_fi->autoIncr = FALSE;
		p_fi->nullOk = FALSE;
		p_fi->status = status;
		p_fi->origin = (p_fieldInfo)NULL;
		p_fi->update = (p_fieldInfo)NULL;
	}
	return p_fi;
}

gboolean checkFieldNameOk (GList * lstFields, GString * fieldName) {
	
	
	return TRUE;
}

GList * generateFieldAlterTable (GList * lstFields, GString * table) {
	GList * currField;
	GList * lstSqlQueries;
	GString * sql;
	
	lstSqlQueries = (GList *)NULL;
	currField = g_list_first(lstFields);
	while (currField != (GList *)NULL) {
		sql = fieldToSQL((p_fieldInfo)currField->data, table);
		if (sql != (GString *)NULL) {
			lstSqlQueries = g_list_append(lstSqlQueries, sql);
		}
		currField = g_list_next(currField);
	}
	
	return lstSqlQueries;
}

p_fieldInfo getFieldByName (GList * lstFields, GString * nameField) {
	GList * currField;
	p_fieldInfo p_fi;
	
	currField = g_list_first(lstFields);
	while (currField != (GList *)NULL) {
		p_fi = (p_fieldInfo)currField->data;
		if (strncmp(p_fi->name->str, nameField->str, p_fi->name->len) == 0) {
			return p_fi;
		}
		currField = g_list_next(currField);
	}
	return (p_fieldInfo)NULL;
}

GString * fieldToSQL (p_fieldInfo p_fi, GString * table) {
	GString * sql;
	
	sql = g_string_new("");
	g_string_printf(sql, "ALTER TABLE `%s`", table->str);
	/* Select by status */
	if (p_fi->status == FI_DEL) { /* Remove field */
		/* ALTER TABLE t2 DROP COLUMN c;*/
		g_string_append_printf(sql, " DROP COLUMN `%s`", p_fi->name->str);
		return sql;
	} else if (p_fi->status == FI_ADD) {
		/* ALTER TABLE t2 ADD c INT UNSIGNED NOT NULL AUTO_INCREMENT */
		/* Set name */
		g_string_append_printf(sql, " ADD COLUMN `%s`", p_fi->name->str);
		/* Set type */
		g_string_append_printf(sql, " %s", arFieldTypes[p_fi->type_idx]->name);
		/* Set Length or Length and decimals or List of value if needed */
		switch(arFieldTypes[p_fi->type_idx]->typeValue) {
			case 1 : /* Length */
				g_string_append_printf(sql, "(%i)", p_fi->length);
				break;
			case 2 : /* Length and decimals */
				g_string_append_printf(sql, "(%i,%i)", p_fi->length, p_fi->decimal);
				break;
			case 3 : /* List of value */
				g_string_append_printf(sql, "(%s)", p_fi->set->str);
				break;
			case 0 : /* None */
			default: 
				break;
		}
		if (arFieldTypes[p_fi->type_idx]->binary && p_fi->binary) {
			g_string_append_printf(sql, " BINARY");
		}
		if (arFieldTypes[p_fi->type_idx]->unsign && p_fi->unsign) {
			g_string_append_printf(sql, " UNSIGNED");
		}
		if (arFieldTypes[p_fi->type_idx]->zerofill && p_fi->zerofill) {
			g_string_append_printf(sql, " ZEROFILL");
		}
		/* Set Null or not */
		if (p_fi->nullOk) {
			g_string_append_printf(sql, " NULL");
		} else {
			g_string_append_printf(sql, " NOT NULL");
		}
		g_string_append_printf(sql, " DEFAULT '%s'", p_fi->def->str);
		if (arFieldTypes[p_fi->type_idx]->autoincr && p_fi->autoIncr) {
			g_string_append_printf(sql, " AUTO_INCREMENT");
		}
		return sql;
	} else if (p_fi->status == FI_MODIFY) {
		/* ALTER TABLE t1 CHANGE b b BIGINT NOT NULL */
		/* Set name */
		g_string_append_printf(sql, " CHANGE `%s` `%s`", p_fi->name->str, p_fi->update->name->str);
		/* Set type */
		g_string_append_printf(sql, " %s", arFieldTypes[p_fi->update->type_idx]->name);
		/* Set Length or Length and decimals or List of value if needed */
		g_printf("Type name(%d) : '%s'\n", p_fi->update->type_idx, arFieldTypes[p_fi->update->type_idx]->name);
		switch(arFieldTypes[p_fi->update->type_idx]->typeValue) {
			case 1 : /* Length */
				g_string_append_printf(sql, "(%i)", p_fi->update->length);
				break;
			case 2 : /* Length and decimals */
				g_string_append_printf(sql, "(%i,%i)", p_fi->update->length, p_fi->update->decimal);
				break;
			case 3 : /* List of value */
				g_string_append_printf(sql, "(%s)", p_fi->update->set->str);
				break;
			case 0 : /* None */
			default: 
				break;
		}
		if (arFieldTypes[p_fi->update->type_idx]->binary && p_fi->update->binary) {
				g_string_append_printf(sql, " BINARY");
		}
		if (arFieldTypes[p_fi->update->type_idx]->unsign && p_fi->update->unsign) {
				g_string_append_printf(sql, " UNSIGNED");
		}
		if (arFieldTypes[p_fi->update->type_idx]->zerofill && p_fi->update->zerofill) {
				g_string_append_printf(sql, " ZEROFILL");
		}
		/* Set Null or not */
		if (p_fi->update->nullOk) {
			g_string_append_printf(sql, " NULL");
		} else {
			g_string_append_printf(sql, " NOT NULL");
		}
		g_string_append_printf(sql, " DEFAULT '%s'", p_fi->def->str);
		if (arFieldTypes[p_fi->update->type_idx]->autoincr && p_fi->update->autoIncr) {
				g_string_append_printf(sql, " AUTO_INCREMENT");
		}
		return sql;
	}
	return (GString *)NULL;
}

/* Manage Indexes infos */

p_indexInfo createIndexInfo (int status) {
	p_indexInfo p_ii;

	p_ii = (p_indexInfo) g_try_malloc(sizeof(indexInfo));
	if (p_ii != (p_indexInfo)NULL) {
		p_ii->name = (GString*)NULL;
		p_ii->primary = TRUE;
		p_ii->unique = TRUE;
		p_ii->fieldsIdx = (GList *)NULL;
		p_ii->status = status;
		p_ii->origin = (p_indexInfo)NULL;
		p_ii->update = (p_indexInfo)NULL;
	}
	return p_ii;
}

p_indexFieldInfo createIndexFieldInfo (int status) {
	p_indexFieldInfo p_ifi;

	p_ifi = (p_indexFieldInfo) g_try_malloc(sizeof(indexFieldInfo));
	if (p_ifi != (p_indexFieldInfo)NULL) {
		p_ifi->name = (GString *)NULL;
		p_ifi->field_ref = (p_fieldInfo)NULL;
		p_ifi->subpart = 0;
		p_ifi->status = status;
	}
	return p_ifi;
}

