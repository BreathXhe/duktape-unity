﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Duktape;
using System;

[Duktape.JSType]
public enum SampleEnum
{
    a, b, c
}

[Duktape.JSType]
public interface ISampleBase
{
    string name { get; }
}

[Duktape.JSType]
public class SampleClass
{
    public delegate void DelegateFoo(string a, string b);
    public delegate void DelegateFoo2(string a, string b);
    public delegate double DelegateFoo4(int a, float b);

    public DelegateFoo delegateFoo1;
    public DelegateFoo2 delegateFoo2;
    public DelegateFoo4 delegateFoo4;

    public Action action1;
    public Action<string> action2;
    public Action[] actions1;

    private string _name;
    private SampleEnum _sampleEnum;

    public string name
    {
        get { return _name; }
    }

    public SampleEnum sampleEnum { get { return _sampleEnum; } }

    public void TestDelegate1()
    {
        if (delegateFoo1 != null)
        {
            delegateFoo1("hello", "delegate");
        }
    }

    public void TestVector3(Vector3 v)
    {
        Debug.Log($"TestVector3({v})");
    }

    public Type TestType1(Type type)
    {
        var ret = type ?? type.BaseType;
        Debug.Log($"[CS] TestType1({type}): {ret}");
        return ret;
    }

    public bool SetEnum(SampleEnum sampleEnum)
    {
        _sampleEnum = sampleEnum;
        return true;
    }

    [JSDoc(
        "简单构造函数测试",
        "@param name 测试字符串",
        "@param additional 测试可变参数"
        )]
    public SampleClass(string name, params string[] additional)
    {
        this._name = name + String.Join("+", additional);
    }

    public int CheckingVA(params int[] args)
    {
        return Sum(args);
    }

    public int CheckingVA2(int b, params int[] args)
    {
        return Sum(args) + b;
    }

    public void MethodOverride()
    {

    }

    public void MethodOverride(int x)
    {

    }

    public void MethodOverride(string x)
    {

    }

    public void MethodOverride(float x, float y)
    {

    }

    public void MethodOverride2(int x)
    {
        
    }

    [JSNaming("MethodOverride2F")]
    public void MethodOverride2(float x)
    {
        
    }

    public int Sum(int[] all)
    {
        var sum = 0;
        if (all != null)
        {
            for (int i = 0, size = all.Length; i < size; i++)
            {
                sum += all[i];
            }
        }
        return sum;
    }
}

[Duktape.JSType]
public struct SampleStruct
{
    // field 
    public int field_a;

    // static field
    public static string static_field_b;

    // readonly property
    public int readonly_property_c { get; }

    // read/write property
    public float readwrite_property_d { get; set; }

    // static read/write property
    public static double static_readwrite_property_d { get; set; }

    public static string StaticMethodWithReturnAndNoOverride(Vector3 a, ref float b, out string[] c) { c = null; return a.ToString(); }

    [Duktape.JSMutable]
    public void ChangeFieldA(int a)
    {
        this.field_a += a;
    }

    // vararg method without override
    // public void VarargMethodWithoutOverride(int a, string[] b, params float[] c) { }

    // public bool MethodWithOutParameter(int a, int b, out int c) { c = a + b; return true; }

    // public bool MethodWithRefParameter(int a, int b, ref int c) { c = a + b + c; return false; }

    // public void Foo(List<int> list) { }

    // public void Foo(int a, string b) { }
    // public void Foo(int a, params string[] b) { }

    // public static void X(string a1, string a2) { }
    // public static void X(string a1, int a2) { }
    // public static void X(string a1, params int[] a2)
    // {
    //     Debug.LogFormat("X var {0}", a2.Length);
    // }
}

public static class SampleStructExtensions
{
    public static void Foo(this SampleStruct self)
    {
    }
}

public class Sample : MonoBehaviour, Duktape.IDuktapeListener
{
    DuktapeVM vm = new DuktapeVM();

    public void OnTypesBinding(DuktapeVM vm)
    {
        // 此处进行手工导入
        // var ctx = vm.context.rawValue;
        // xxx.reg(ctx);
    }

    public void OnBindingError(DuktapeVM vm, Type type)
    {
    }

    public void OnProgress(DuktapeVM vm, int step, int total)
    {
    }

    public void OnLoaded(DuktapeVM vm)
    {
        // DuktapeDebugger.CreateDebugger(vm.context.rawValue);
        vm.AddSearchPath("Assets/Scripts/polyfills");
        vm.AddSearchPath("Assets/Scripts/Generated");
        vm.EvalFile("console-minimal.js");
        vm.EvalFile("scratch.js");
        vm.EvalMain("main.js");
    }

    void Awake()
    {
        vm.Initialize(new FakeFileSystem(), this);
    }

    void OnDestroy()
    {
        // DuktapeDebugger.Shutdown();
        vm.Destroy();
        vm = null;
    }
}
