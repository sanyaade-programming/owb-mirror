#!/usr/bin/perl -w
#
# Copyright (C) 2010 Chris Jerdonek (chris.jerdonek@gmail.com)
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Unit tests of parseDiff().

use strict;
use warnings;

use Test::More;
use VCSUtils;

# The array of test cases.
my @testCaseHashRefs = (
{
    # New test
    diffName => "SVN: simple",
    inputText => <<'END',
Index: WebKitTools/Makefile
===================================================================
--- WebKitTools/Makefile	(revision 53052)
+++ WebKitTools/Makefile	(working copy)
@@ -1,3 +1,4 @@
+
 MODULES = JavaScriptCore JavaScriptGlue WebCore WebKit WebKitTools
 
 all:
END
    expectedReturn => [
[{
    svnConvertedText =>  <<'END', # Same as input text
Index: WebKitTools/Makefile
===================================================================
--- WebKitTools/Makefile	(revision 53052)
+++ WebKitTools/Makefile	(working copy)
@@ -1,3 +1,4 @@
+
 MODULES = JavaScriptCore JavaScriptGlue WebCore WebKit WebKitTools
 
 all:
END
    indexPath => "Makefile",
    isSvn => 1,
    sourceRevision => "53052",
}],
undef],
    expectedNextLine => undef,
},
{
    # New test
    diffName => "SVN: binary file (isBinary true)",
    inputText => <<'END',
Index: WebKitTools/test_file.swf
===================================================================
Cannot display: file marked as a binary type.
svn:mime-type = application/octet-stream

Property changes on: test_file.swf
___________________________________________________________________
Name: svn:mime-type
   + application/octet-stream


Q1dTBx0AAAB42itg4GlgYJjGwMDDyODMxMDw34GBgQEAJPQDJA==
END
    expectedReturn => [
[{
    svnConvertedText =>  <<'END', # Same as input text
Index: WebKitTools/test_file.swf
===================================================================
Cannot display: file marked as a binary type.
svn:mime-type = application/octet-stream

Property changes on: test_file.swf
___________________________________________________________________
Name: svn:mime-type
   + application/octet-stream


Q1dTBx0AAAB42itg4GlgYJjGwMDDyODMxMDw34GBgQEAJPQDJA==
END
    indexPath => "test_file.swf",
    isBinary => 1,
    isSvn => 1,
}],
undef],
    expectedNextLine => undef,
},
{
    # New test
    diffName => "SVN: leading junk",
    inputText => <<'END',

LEADING JUNK

Index: WebKitTools/Makefile
===================================================================
--- WebKitTools/Makefile	(revision 53052)
+++ WebKitTools/Makefile	(working copy)
@@ -1,3 +1,4 @@
+
 MODULES = JavaScriptCore JavaScriptGlue WebCore WebKit WebKitTools
 
 all:
END
    expectedReturn => [
[{
    svnConvertedText =>  <<'END', # Same as input text

LEADING JUNK

Index: WebKitTools/Makefile
===================================================================
--- WebKitTools/Makefile	(revision 53052)
+++ WebKitTools/Makefile	(working copy)
@@ -1,3 +1,4 @@
+
 MODULES = JavaScriptCore JavaScriptGlue WebCore WebKit WebKitTools
 
 all:
END
    indexPath => "Makefile",
    isSvn => 1,
    sourceRevision => "53052",
}],
undef],
    expectedNextLine => undef,
},
{
    # New test
    diffName => "SVN: copied file",
    inputText => <<'END',
Index: WebKitTools/Makefile_new
===================================================================
--- WebKitTools/Makefile_new	(revision 53131)	(from Makefile:53131)
+++ WebKitTools/Makefile_new	(working copy)
@@ -0,0 +1,1 @@
+MODULES = JavaScriptCore JavaScriptGlue WebCore WebKit WebKitTools
END
    expectedReturn => [
[{
    copiedFromPath => "Makefile",
    indexPath => "Makefile_new",
    sourceRevision => "53131",
}],
undef],
    expectedNextLine => undef,
},
{
    # New test
    diffName => "SVN: two diffs",
    inputText => <<'END',
Index: WebKitTools/Makefile
===================================================================
--- WebKitTools/Makefile	(revision 53131)
+++ WebKitTools/Makefile	(working copy)
@@ -1,1 +0,0 @@
-MODULES = JavaScriptCore JavaScriptGlue WebCore WebKit WebKitTools
Index: WebKitTools/Makefile_new
===================================================================
--- WebKitTools/Makefile_new	(revision 53131)	(from Makefile:53131)
END
    expectedReturn => [
[{
    svnConvertedText =>  <<'END',
Index: WebKitTools/Makefile
===================================================================
--- WebKitTools/Makefile	(revision 53131)
+++ WebKitTools/Makefile	(working copy)
@@ -1,1 +0,0 @@
-MODULES = JavaScriptCore JavaScriptGlue WebCore WebKit WebKitTools
END
    indexPath => "Makefile",
    isSvn => 1,
    sourceRevision => "53131",
}],
"Index: WebKitTools/Makefile_new\n"],
    expectedNextLine => "===================================================================\n",
},
{
    # New test
    diffName => "SVN: SVN diff followed by Git diff", # Should not recognize Git start
    inputText => <<'END',
Index: WebKitTools/Makefile
===================================================================
--- WebKitTools/Makefile	(revision 53131)
+++ WebKitTools/Makefile	(working copy)
@@ -1,1 +0,0 @@
-MODULES = JavaScriptCore JavaScriptGlue WebCore WebKit WebKitTools
diff --git a/Makefile b/Makefile
index f5d5e74..3b6aa92 100644
--- WebKitTools/a/Makefile
+++ WebKitTools/b/Makefile
@@ -1,1 1,1 @@ public:
END
    expectedReturn => [
[{
    svnConvertedText =>  <<'END', # Same as input text
Index: WebKitTools/Makefile
===================================================================
--- WebKitTools/Makefile	(revision 53131)
+++ WebKitTools/Makefile	(working copy)
@@ -1,1 +0,0 @@
-MODULES = JavaScriptCore JavaScriptGlue WebCore WebKit WebKitTools
diff --git a/Makefile b/Makefile
index f5d5e74..3b6aa92 100644
--- WebKitTools/a/Makefile
+++ WebKitTools/b/Makefile
@@ -1,1 1,1 @@ public:
END
    indexPath => "Makefile",
    isSvn => 1,
    sourceRevision => "53131",
}],
undef],
    expectedNextLine => undef,
},
####
#    Git test cases
##
{
    # New test
    diffName => "Git: simple",
    inputText => <<'END',
diff --git a/Makefile b/Makefile
index f5d5e74..3b6aa92 100644
--- WebKitTools/a/Makefile
+++ WebKitTools/b/Makefile
@@ -1,1 1,1 @@ public:
END
    expectedReturn => [
[{
    svnConvertedText =>  <<'END',
Index: WebKitTools/Makefile
index f5d5e74..3b6aa92 100644
--- WebKitTools/Makefile
+++ WebKitTools/Makefile
@@ -1,1 1,1 @@ public:
END
    indexPath => "Makefile",
    isGit => 1,
}],
undef],
    expectedNextLine => undef,
},
{   # New test
    diffName => "Git: new file",
    inputText => <<'END',
diff --git a/foo.h b/foo.h
new file mode 100644
index 0000000..3c9f114
--- WebKitTools//dev/null
+++ WebKitTools/b/foo.h
@@ -0,0 +1,34 @@
+<html>
diff --git a/bar b/bar
index d45dd40..3494526 100644
END
    expectedReturn => [
[{
    svnConvertedText => <<'END',
Index: WebKitTools/foo.h
new file mode 100644
index 0000000..3c9f114
--- WebKitTools/foo.h
+++ WebKitTools/foo.h
@@ -0,0 +1,34 @@
+<html>
END
    indexPath => "foo.h",
    isGit => 1,
    isNew => 1,
}],
"diff --git a/bar b/bar\n"],
    expectedNextLine => "index d45dd40..3494526 100644\n",
},
{   # New test
    diffName => "Git: file deletion",
    inputText => <<'END',
diff --git a/foo b/foo
deleted file mode 100644
index 1e50d1d..0000000
--- WebKitTools/a/foo
+++ WebKitTools//dev/null
@@ -1,1 +0,0 @@
-line1
diff --git a/bar b/bar
index d45dd40..3494526 100644
END
    expectedReturn => [
[{
    svnConvertedText => <<'END',
Index: WebKitTools/foo
deleted file mode 100644
index 1e50d1d..0000000
--- WebKitTools/foo
+++ WebKitTools/foo
@@ -1,1 +0,0 @@
-line1
END
    indexPath => "foo",
    isDeletion => 1,
    isGit => 1,
}],
"diff --git a/bar b/bar\n"],
    expectedNextLine => "index d45dd40..3494526 100644\n",
},
{
    # New test
    diffName => "Git: Git diff followed by SVN diff", # Should not recognize SVN start
    inputText => <<'END',
diff --git a/Makefile b/Makefile
index f5d5e74..3b6aa92 100644
--- WebKitTools/a/Makefile
+++ WebKitTools/b/Makefile
@@ -1,1 1,1 @@ public:
Index: WebKitTools/Makefile_new
===================================================================
--- WebKitTools/Makefile_new	(revision 53131)	(from Makefile:53131)
END
    expectedReturn => [
[{
    svnConvertedText =>  <<'END',
Index: WebKitTools/Makefile
index f5d5e74..3b6aa92 100644
--- WebKitTools/Makefile
+++ WebKitTools/Makefile
@@ -1,1 1,1 @@ public:
Index: WebKitTools/Makefile_new
===================================================================
--- WebKitTools/Makefile_new	(revision 53131)	(from Makefile:53131)
END
    indexPath => "Makefile",
    isGit => 1,
}],
undef],
    expectedNextLine => undef,
},
####
#    Git test cases: file moves (multiple return values)
##
{
    diffName => "Git: rename (with similarity index 100%)",
    inputText => <<'END',
diff --git a/foo b/foo_new
similarity index 100%
rename from foo
rename to foo_new
diff --git a/bar b/bar
index d45dd40..3494526 100644
END
    expectedReturn => [
[{
    indexPath => "foo",
    isDeletion => 1,
},
{
    copiedFromPath => "foo",
    indexPath => "foo_new",
}],
"diff --git a/bar b/bar\n"],
    expectedNextLine => "index d45dd40..3494526 100644\n",
},
{
    diffName => "rename (with similarity index < 100%)",
    inputText => <<'END',
diff --git a/foo b/foo_new
similarity index 99%
rename from foo
rename to foo_new
index 1e50d1d..1459d21 100644
--- WebKitTools/a/foo
+++ WebKitTools/b/foo_new
@@ -15,3 +15,4 @@ release r deployment dep deploy:
 line1
 line2
 line3
+line4
diff --git a/bar b/bar
index d45dd40..3494526 100644
END
    expectedReturn => [
[{
    indexPath => "foo",
    isDeletion => 1,
},
{
    copiedFromPath => "foo",
    indexPath => "foo_new",
},
{
    indexPath => "foo_new",
    isGit => 1,
    svnConvertedText => <<'END',
Index: WebKitTools/foo_new
similarity index 99%
rename from foo
rename to foo_new
index 1e50d1d..1459d21 100644
--- WebKitTools/foo_new
+++ WebKitTools/foo_new
@@ -15,3 +15,4 @@ release r deployment dep deploy:
 line1
 line2
 line3
+line4
END
}],
"diff --git a/bar b/bar\n"],
    expectedNextLine => "index d45dd40..3494526 100644\n",
},
{
    diffName => "rename (with executable bit change)",
    inputText => <<'END',
diff --git a/foo b/foo_new
old mode 100644
new mode 100755
similarity index 100%
rename from foo
rename to foo_new
diff --git a/bar b/bar
index d45dd40..3494526 100644
END
    expectedReturn => [
[{
    indexPath => "foo",
    isDeletion => 1,
},
{
    copiedFromPath => "foo",
    indexPath => "foo_new",
},
{
    indexPath => "foo_new",
    isGit => 1,
    svnConvertedText => <<'END',
Index: WebKitTools/foo_new
old mode 100644
new mode 100755
similarity index 100%
rename from foo
rename to foo_new
END
}],
"diff --git a/bar b/bar\n"],
    expectedNextLine => "index d45dd40..3494526 100644\n",
},
);

my $testCasesCount = @testCaseHashRefs;
plan(tests => 2 * $testCasesCount); # Total number of assertions.

foreach my $testCase (@testCaseHashRefs) {
    my $testNameStart = "parseDiff(): $testCase->{diffName}: comparing";

    my $fileHandle;
    open($fileHandle, "<", \$testCase->{inputText});
    my $line = <$fileHandle>;

    my @got = VCSUtils::parseDiff($fileHandle, $line);
    my $expectedReturn = $testCase->{expectedReturn};

    is_deeply(\@got, $expectedReturn, "$testNameStart return value.");

    my $gotNextLine = <$fileHandle>;
    is($gotNextLine, $testCase->{expectedNextLine},  "$testNameStart next read line.");
}
