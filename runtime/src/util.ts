import minimist from 'minimist';
import minimistOptions from 'minimist-options';
export * from './format';

export function flags(argv: string[], options?: any) {
    options = options || {};
    return minimist(argv, minimistOptions(options));
}
//export const plflagsOptions = minimistOptions;