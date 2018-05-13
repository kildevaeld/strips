export { }

declare global {
    export class Proxy {
        constructor(target: any, traps: any);
    }

    interface ObjectConstructor {
        assign(...args: any[]): any;
    }

    export module Duktape {
        export function enc(enc: string, a: any): string;
    }

}

