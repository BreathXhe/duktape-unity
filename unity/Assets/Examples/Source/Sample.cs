using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Duktape;
using System;

public class Sample : MonoBehaviour, IDuktapeListener
{
    public string launchScript = "code.js";
    public bool experimentalDebugger = false;
    public bool waitForDebuggerClient = false;
    public bool jsBytecode = false;

    private bool _loaded;
    DuktapeVM vm = new DuktapeVM();

    public void OnBinded(DuktapeVM vm, int numRegs)
    {
        if (numRegs == 0)
        {
            throw new Exception("no type binding registered, please run <MENU>/Duktape/Generate Bindings in Unity Editor Mode before the first running of this project.");
        }
    }

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
        tests();
        vm.AddSearchPath("Assets/Examples/Scripts/out");
        do
        {
            if (experimentalDebugger)
            {
                // DuktapeDLL.duk_example_attach_debugger(vm.context.rawValue);
                DuktapeDebugger.CreateDebugger(vm);
                if (waitForDebuggerClient)
                {
                    Debug.Log("wait for debugger connection");
                    DuktapeDebugger.onAttached = () =>
                    {
                        DuktapeDebugger.onAttached = null;
                        RunScript(vm);
                    };
                    break;
                }
            }
            RunScript(vm);
        } while (false);
        // vm.EvalFile("test.js");
        _loaded = true;
    }

    private void RunScript(DuktapeVM vm)
    {
        if (jsBytecode)
        {
            var fr = vm.fileResolver;
            var bytecode = fr.ReadAllBytes(launchScript + ".bytes");
            if (bytecode != null)
            {
                vm.EvalMain(launchScript, bytecode);
            }
            else
            {
                vm.EvalMain(launchScript);
            }
        }
        else
        {
            vm.EvalMain(launchScript);
        }
    }

    private void tests()
    {
        {
            var start = DateTime.Now;
            var v = new Vector3(0, 0, 0);
            for (var i = 1; i < 200000; i++)
            {
                v.Set(i, i, i);
                v.Normalize();
            }
            Debug.LogFormat("c#/vector3/normalize {0}", (DateTime.Now - start).TotalSeconds);
        }
        {
            var start = DateTime.Now;
            var v = Vector3.zero;
            var w = Vector3.one;
            for (var i = 1; i < 200000; i++)
            {
                v.Scale(w);
            }
            Debug.LogFormat("c#/vector3/scale {0}", (DateTime.Now - start).TotalSeconds);
        }
        {
            var start = DateTime.Now;
            var sum = 0;
            for (var i = 1; i < 20000000; i++)
            {
                sum += i;
            }
            Debug.LogFormat("c#/number/add {0} {1}", (DateTime.Now - start).TotalSeconds, sum);
        }
    }

    public void OnSourceModified()
    {
        if (_loaded)
        {
            // vm.context.Invoke("OnBeforeSourceReload");
            // vm.context.Invoke("OnAfterSourceReload");
        }
    }

    void Awake()
    {
        vm.Initialize(this);
    }

    // void Update()
    // {
    //     Debug.LogFormat("Update");
    // }

    // void OnApplicationQuit()
    // {
    //     Debug.LogFormat("OnApplicationQuit");
    // }

    void OnDestroy()
    {
        vm.Destroy();
        vm = null;
    }
}
