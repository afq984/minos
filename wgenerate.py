import pwd
import grp
import spwd


print('import pwd, grp, spwd, unittest')
print('class DbTest(unittest.TestCase):')
pwlist = [pw for pw in pwd.getpwall() if pw.pw_uid >= 1000]
print('    def test_getpwnam(self):')
for pw in pwlist:
    print(f'        self.assertEqual({tuple(pw)}, tuple(pwd.getpwnam({pw.pw_name!r})))')
print('    def test_getpwuid(self):')
for pw in pwlist:
    print(f'        self.assertEqual({tuple(pw)}, tuple(pwd.getpwuid({pw.pw_uid!r})))')
grlist = [gr for gr in grp.getgrall() if gr.gr_gid >= 1000]
print('    def test_getgrnam(self):')
for gr in grlist:
    print(f'        self.assertEqual({tuple(gr)}, tuple(grp.getgrnam({gr.gr_name!r})))')
print('    def test_getgrgid(self):')
for gr in grlist:
    print(f'        self.assertEqual({tuple(gr)}, tuple(grp.getgrgid({gr.gr_gid!r})))')
splist = [sp for sp in spwd.getspall() if pwd.getpwnam(sp.sp_namp).pw_uid >= 1000]
print('    def test_getspnam(self):')
for sp in splist:
    print(f'        self.assertEqual({tuple(sp)}, tuple(spwd.getspnam({sp.sp_namp!r})))')
print('        pass')
print('unittest.main()')
