
#if defined _INC_y_hooks
	#error Not compatible with y_hooks
#endif

#include <YSI-Coding\y_unique>

#if defined _pp_hooks_included
	#endinput
#endif
#define _pp_hooks_included

native pawn_call_public_s(ConstAmxString:function, const format[], AnyTag:...) = pawn_call_public;
native CallbackHandler:pawn_register_callback_s(ConstAmxString:callback, ConstAmxString:handler, handler_flags:flags=handler_default, const additional_format[]="", AnyTag:...) = pawn_register_callback;
native NativeHook:pawn_add_hook_s(ConstAmxString:function, const format[], ConstAmxString:handler, const additional_format[]="", AnyTag:...) = pawn_add_hook;
native pawn_public_exists_s(ConstAmxString:function) = pawn_public_exists;
native pawn_create_callback_s(ConstAmxString:callback, Expression:action) = pawn_create_callback;

#define @pH_public%0\32;%1(%2);%3(%4) @pH_%0%1(%2);@pH_%0%1(%2)
#define @pH_native%0\32;%1(%2);%3(%4) @Hp_%0%1(%2);@Hp_%0%1(%2)
#define @pH_ret%0\32;%1(%2);%3(%4) @p_H%0%1(%2);@p_H%0%1(%2)

#define hook%1(%2) forward UNIQUE_FUNCTION<@pH_%1@...>(%2);UNIQUE_FUNCTION<@pH_%1@...>(%2)

#if !defined PP_DEFAULT_PUBLIC_HOOK_METHOD
	#define PP_DEFAULT_PUBLIC_HOOK_METHOD handler_default
#endif

// Strip spaces from the generated function names.
#define @pH_%0\32; @pH_
#define @_pH%0\32; @_pH
#define @Hp_%0\32; @Hp_
#define @p_H%0\32; @p_H

#define DEFINE_HOOK_REPLACEMENT__(%0,%1); forward @_pH%0(); public @_pH%0() { _PP_HOOKS_Replacement(#%0, #%1); }

// Create the default replacements.
DEFINE_HOOK_REPLACEMENT__(Checkpoint, CP )
DEFINE_HOOK_REPLACEMENT__(Container , Cnt)
DEFINE_HOOK_REPLACEMENT__(Inventory , Inv)
DEFINE_HOOK_REPLACEMENT__(Dynamic   , Dyn)
DEFINE_HOOK_REPLACEMENT__(TextDraw  , TD )
DEFINE_HOOK_REPLACEMENT__(Update    , Upd)
DEFINE_HOOK_REPLACEMENT__(Object    , Obj)
DEFINE_HOOK_REPLACEMENT__(Command   , Cmd)
DEFINE_HOOK_REPLACEMENT__(DynamicCP , DynamicCP)

static List:ReplacementList;

forward _pp@on_init@PPHooks();
public _pp@on_init@PPHooks()
{
	#if defined PPHOOKS_WAIT_TICK
	wait_ticks(1);
	#endif

	new const
		public_count = amx_num_publics()
	;

	ReplacementList = list_new();

	// Check if script is dealing with SAMPGDK incompatibility
	{
		new const original = pp_public_min_index(0);

		if(original == -1)
		{
			print("[PPHooks] pp_public_min_index forced to 0 to evade SAMPGDK incompatibility");
		}
	}

	// Set up repalcements
	for(new i; i < public_count; ++i)
	{
		new String:public_name = amx_public_name_s(i);

		if(str_match(public_name, "^@_pH"))
		{
			pawn_call_public(amx_encode_public(i), "");
		}

		str_delete(public_name);
	}

	// Sort replacements list
	list_sort_expr(ReplacementList, expr_parse("str_len($arg0[1]) > str_len($arg1[1])"));

	// Check existance and call replacements
	for(new i; i < public_count; ++i)
	{
		new String:public_name = amx_public_name_s(i);

		// Public hook
		new List:regex_results = str_extract(public_name, "^@pH_(.*?)@.*$");
		if(regex_results)
		{
			new String:callback = list_get_str_s(regex_results, 1);
			PPHooks_DoReplacement(callback);

			if(!pawn_public_exists_s(callback))
			{
				pawn_create_callback_s(callback, expr_false());
			}

			list_delete_deep(regex_results);
		}
		else if((regex_results = str_extract(public_name, "^@p_H(.*?)@.*$"))) // Ret hook
		{
			new String:callback = list_get_str_s(regex_results, 1);
			PPHooks_DoReplacement(callback);

			if(!pawn_public_exists_s(callback))
			{
				pawn_create_callback_s(callback, expr_false());
			}

			list_delete_deep(regex_results);
		}

		str_delete(public_name);
	}

	// Find everything
	for(new i; i < public_count; ++i)
	{
		new String:public_name = amx_public_name_s(i);

		// Public hook
		new List:regex_results = str_extract(public_name, "^@pH_(.*?)@.*$");
		if(regex_results)
		{
			new String:callback = list_get_str_s(regex_results, 1);
			PPHooks_DoReplacement(callback);

			pawn_register_callback_s(callback, public_name, PP_DEFAULT_PUBLIC_HOOK_METHOD);

			list_delete(regex_results);
		}
		else if((regex_results = str_extract(public_name, "^@p_H(.*?)@.*$"))) // Ret hook
		{
			new String:callback = list_get_str_s(regex_results, 1);
			PPHooks_DoReplacement(callback);

			pawn_register_callback_s(callback, public_name, handler_return);

			list_delete(regex_results);
		}
		else if((regex_results = str_extract(public_name, "^@Hp_(.*?)@.*$"))) // Native hook
		{
			new String:callback = list_get_str_s(regex_results, 1);
			PPHooks_DoReplacement(callback);

			pawn_add_hook_s(callback, "-", public_name);

			list_delete(regex_results);
		}

		str_delete(public_name);
	}
	return 1;
}


PPHooks_DoReplacement(String:callback)
{
	for(new Iter:i=list_iter(ReplacementList);iter_inside(i);iter_move_next(i))
	{
		new String:longName = String:iter_get(i, 0), String:shortName = String:iter_get(i, 1);

		new pos = str_find(callback, shortName, pos);
		if(pos != -1 )
		{
			new character = str_getc(callback, pos + str_len(shortName));
			if(('a' <= character <= 'z'))
			{
				continue;
			}

			str_set_replace_s(callback, callback, shortName, longName, pos);
			break;
		}
	}
}

_PP_HOOKS_Replacement(const longName[], const shortName[])
{
	new String:array[2];
	array[0] = str_new(longName);
	array[1] = str_new(shortName);

	list_add_arr(ReplacementList, array);
}
