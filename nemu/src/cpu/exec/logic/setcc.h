#ifndef __SETCC_H__
#define __SETCC_H__

#define make_set(name) make_helper(concat3(set, name, _rm_b))

make_set(a);
make_set(ae);
make_set(b);
make_set(be);
make_set(e);
make_set(g);
make_set(ge);
make_set(l);
make_set(le);
make_set(ne);
make_set(no);
make_set(np);
make_set(ns);
make_set(o);
make_set(p);
make_set(s);

#endif
