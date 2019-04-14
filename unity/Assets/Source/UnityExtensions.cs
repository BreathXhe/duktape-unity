using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Duktape;
using System;

[Duktape.JSType]
public class UnityExtensions
{
    public static bool RaycastMousePosition(DuktapeObject outValue, float maxDistance, int layerMask)
    {
        var ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hitInfo;
        var res = Physics.Raycast(ray, out hitInfo, maxDistance, layerMask);
        if (outValue != null)
        {
            var ctx = outValue.ctx;
            if (outValue.Push(ctx))
            {
                DuktapeBinding.duk_push_any(ctx, hitInfo.collider);
                DuktapeDLL.duk_put_prop_string(ctx, -2, "collider");
                
                DuktapeBinding.duk_push_any(ctx, hitInfo.point);
                DuktapeDLL.duk_put_prop_string(ctx, -2, "point");
                
                DuktapeBinding.duk_push_any(ctx, hitInfo.normal);
                DuktapeDLL.duk_put_prop_string(ctx, -2, "normal");
                
                DuktapeBinding.duk_push_any(ctx, hitInfo.distance);
                DuktapeDLL.duk_put_prop_string(ctx, -2, "distance");
                
                DuktapeBinding.duk_push_any(ctx, hitInfo.transform);
                DuktapeDLL.duk_put_prop_string(ctx, -2, "transform");
                
                DuktapeBinding.duk_push_any(ctx, hitInfo.textureCoord);
                DuktapeDLL.duk_put_prop_string(ctx, -2, "textureCoord");
                
                DuktapeBinding.duk_push_any(ctx, hitInfo.triangleIndex);
                DuktapeDLL.duk_put_prop_string(ctx, -2, "triangleIndex");
                
                DuktapeDLL.duk_pop(ctx);
            }
        }
        return res;
    }
}
