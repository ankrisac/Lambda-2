"use strict";

var Module = {
    preRun: [],
    postRun: [],

    print: (text) => { console.log(text); },
    printErr: (text) => { console.error(text); },
    
    totalDependencies: 0,

    onRuntimeInitialized: () => {
        console.log("INITIALIZED");

        
        let Compiler = {
            init: Module.cwrap('M_CompilerJS_init', null, []),
            quit: Module.cwrap('M_CompilerJS_quit', null, []),
            parse: Module.cwrap('M_CompilerJS_parse', null, ['string']),
            color: Module.cwrap('M_CompilerJS_color_HTML', 'string', []),
            error: Module.cwrap('M_CompilerJS_get_errors', 'string', []),
            print_tree: Module.cwrap('M_CompilerJS_print_tree', null, []),
        };
        Compiler.init();
        
        let editor_in = document.getElementById('editor_in');
        let editor_out = document.getElementById('editor_out');
        let editor_err = document.getElementById('editor_err');

        function editor_update(){
            try{
                /*  Chrome does inplace string replacement which 
                    is against the ECMAScript standard, so we need to explictly copy
                    Also first div is omitted for some absurd 
                    reason in contenteditable divs */

                Compiler.parse(
                    (' ' + editor_in.innerHTML)
                        .slice(1)
                        .replace(/^[^<>]+/, m => m + '\n')
                        .replace(/<\/div>/g, '\n')
                        .replace(/<[^>]*>/g, '')
                        .replace(/&gt;/g, '>').replace(/&lt;/g, '<')
                        .replace(/\n$/, '')
                );
                
                editor_out.innerHTML = Compiler.color();
                editor_err.innerHTML = Compiler.error();
            }
            catch(ex){
                console.log("ERRORS: ", ex);
            }
        };
        editor_in.onkeydown = (e) => {
            let keyCode = e.keyCode || e.which;

            if(e.keyCode == 9){
                e.preventDefault();

                let sel = window.getSelection();
                var range = sel.getRangeAt(0);

                var tabNode = document.createTextNode("    ");
                range.insertNode(tabNode);

                range.setStartAfter(tabNode);
                range.setEndAfter(tabNode); 
                sel.removeAllRanges();
                sel.addRange(range);
                editor_update();
            }
        };

        editor_in.oninput = editor_update;
        editor_update();
    }
}