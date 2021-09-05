#ifndef __JCC_H__
#define __JCC_H__
#define make_b(name) make_helper(concat(name, _si_b))
#define make_v(name) make_helper(concat(name, _si_v))

make_b(ja);
make_b(jae);
make_b(jb);
make_b(jbe);
make_b(jc);
make_b(jecxz);
make_b(je);
make_b(jg);
make_b(jge);
make_b(jl);
make_b(jle);
make_b(jne);
make_b(jno);
make_b(jnp);
make_b(jns);
make_b(jo);
make_b(jp);
make_b(js);

make_v(ja);
make_v(jae);
make_v(jb);
make_v(jbe);
make_v(je);
make_v(jg);
make_v(jge);
make_v(jl);
make_v(jle);
make_v(jne);
make_v(jno);
make_v(jnp);
make_v(jns);
make_v(jo);
make_v(jp);
make_v(js);



#endif