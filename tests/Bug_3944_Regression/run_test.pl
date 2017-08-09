

eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
     & eval 'exec perl -S $0 $argv:q'
     if 0;

# -*- perl -*-

use lib "$ENV{ACE_ROOT}/bin";
use PerlACE::TestTarget;
use File::Spec;

$status =0;

open (OLDOUT, ">&STDOUT");
open (STDOUT, ">" . File::Spec->devnull());
open (OLDERR, ">&STDERR");
open (STDERR, ">&STDOUT");

my $tao_idl = "$ENV{ACE_ROOT}/bin/tao_idl";
if (exists $ENV{HOST_ROOT}) {
    $tao_idl = "$ENV{HOST_ROOT}/bin/tao_idl";
}

my $server = PerlACE::TestTarget::create_target (1) || die "Create target 1 failed\n";

$input_file1 = $server->LocalFile ("connector_inst_conn.idl");

# Compile the IDL
$SV = $server->CreateProcess ("$tao_idl", "$input_file1");

$server_status1 = $SV->SpawnWaitKill ($server->ProcessStartWaitInterval());

open (STDOUT, ">&OLDOUT");
open (STDERR, ">&OLDERR");

if ($server_status1 == 0) {
    print STDERR "ERROR: tao_idl returned $server_status1 for $input_file1, should have failed\n";
    $status = 1;
}

exit $status;
