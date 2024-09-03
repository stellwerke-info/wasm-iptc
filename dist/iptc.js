var _a;
class Iptc {
    static #wasm_file;
    static #wasm = null;
    #wfn = null;
    constructor(wasm_file = null) {
        if (wasm_file !== null)
            _a.#wasm_file = wasm_file;
        else
            _a.#wasm_file = import.meta.url.replace(/\.js$/, '.wasm');
    }
    _iptc_read_obj;
    _jpeg_app_parts;
    #iptcparse_cb(dataset, recnum, data, data_len) {
        const key = dataset.toString() + '#' + recnum.toString();
        if (!this._iptc_read_obj.has(key)) {
            this._iptc_read_obj.set(key, []);
        }
        this._iptc_read_obj.get(key).push(this.#buf_from_ptr_len(data, data_len));
    }
    #read_app_cb(app_num, data, data_len) {
        this._jpeg_app_parts.set(app_num, [data, data_len]);
    }
    async #load_wasm() {
        if (this.#wfn != null)
            return;
        const importObject = {
            env: {
                iptcparse_cb: this.#iptcparse_cb.bind(this),
                read_app_cb: this.#read_app_cb.bind(this),
            }
        };
        if (_a.#wasm == null)
            _a.#wasm = await (await fetch(_a.#wasm_file)).arrayBuffer();
        const wasm = await WebAssembly.instantiate(_a.#wasm, importObject);
        this.#wfn = wasm.instance.exports;
    }
    #get_memory() {
        return new Uint8Array(this.#wfn.memory.buffer);
    }
    #buf_from_ptr_len(ptr, len) {
        return this.#get_memory().slice(ptr, ptr + len);
    }
    #buf_from_file_ptr(file_ptr) {
        const len = this.#wfn.file_len(file_ptr);
        const ptr = this.#wfn.file_ptr(file_ptr);
        const buf = this.#get_memory().slice(ptr, ptr + len);
        console.assert(buf.length == len);
        return buf;
    }
    async alloc_from_buf(data) {
        await this.#load_wasm();
        const buf = new Uint8Array(data);
        const ptr = this.#wfn.alloc_buf(buf.length);
        this.#get_memory().subarray(ptr).set(buf);
        return [ptr, buf.length];
    }
    async iptcparse(jpeg_ptr) {
        await this.#load_wasm();
        this._iptc_read_obj = new Map();
        this._jpeg_app_parts = new Map();
        const app_res = this.#wfn.jpeg_iter_app(...jpeg_ptr);
        if (app_res && this._jpeg_app_parts.has(13)) {
            this.#wfn.iptcparse(...this._jpeg_app_parts.get(13));
        }
        const iptc = this._iptc_read_obj;
        this._iptc_read_obj = null;
        this._jpeg_app_parts = null;
        return iptc;
    }
    async iptcembed(jpeg_ptr, tags_map) {
        await this.#load_wasm();
        const buf = [];
        for (const [key, values] of tags_map) {
            let [type, code] = key.split('#').map(Number);
            for (let tag_value of values) {
                const tag_size = [tag_value.length >> 8, tag_value.length & 0xff];
                buf.push(0x1c, type, code, ...tag_size, ...tag_value);
            }
        }
        const iptc_ptr = await this.alloc_from_buf(buf);
        const out_ptr = this.#wfn.iptcembed(...iptc_ptr, ...jpeg_ptr);
        if (out_ptr == 0) {
            return null;
        }
        return this.#buf_from_file_ptr(out_ptr);
    }
}
_a = Iptc;
export { Iptc };
