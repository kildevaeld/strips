const Koa = require('koa'),
    Router = require('koa-router');


const app = new Koa();

const router = new Router();

router.get('/', async (ctx) => {
    ctx.body = "Hello, World";
});

router.post('/post', async (ctx) => {
    const r = ctx.req;
    console.log(r.method, r.url, r.headers);
    var body = "";
    let p = new Promise((res, rej) => {
        r.on('data', function (data) {
            body += data.toString();
        });
        r.on('end', function () {

            res(body)
        });
    });
    ctx.body = await p;
})


app.use(router.middleware());
app.listen(3000);
console.log('started');