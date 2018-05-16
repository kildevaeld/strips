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
    r.on('readable', function () {
        body += r.read();
    });
    r.on('end', function () {
        console.log(body);
    });
})


app.use(router.middleware());
app.listen(3000);
console.log('started');