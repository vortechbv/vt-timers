! Copyright (c) 2019-2020 VORtech b.v.
!
! Permission is hereby granted, free of charge, to any person obtaining a copy
! of this software and associated documentation files (the "Software"), to deal
! in the Software without restriction, including without limitation the rights
! to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
! copies of the Software, and to permit persons to whom the Software is
! furnished to do so, subject to the following conditions:
!
! The above copyright notice and this permission notice shall be included in
! all copies or substantial portions of the Software.
!
! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
! FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
! AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
! LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
! OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
! SOFTWARE. 
module vt_timers
implicit none
  integer, parameter :: vtOK = 0, vtWARNING = 1, vtERROR = 2

interface
     function vt_last_error_code() result(ierr) bind(c,name='vt_last_error_code')  
     use iso_c_binding
     implicit none
        integer(kind=c_int) :: ierr
     end function vt_last_error_code


     function vt_timers_to_stdout() result(ierr) bind(c,name='vt_timers_to_stdout')
     use iso_c_binding
     implicit none
        integer(kind=c_int) :: ierr
     end function vt_timers_to_stdout

     function vt_timers_reset() result(ierr) bind(c,name='vt_timers_reset')
     use iso_c_binding
     implicit none
        integer(kind=c_int) :: ierr
     end function vt_timers_reset

     subroutine c_vt_last_error_message(cstring, n) bind(c,name='vt_last_error_message') 
     use iso_c_binding
     implicit none
        character(kind=c_char), intent(out)        :: cstring(*)
        integer(kind=c_size_t), intent(in), value  :: n
     end subroutine c_vt_last_error_message

     function c_vt_timer_tic(name) result(ierr) bind(c,name='vt_timer_tic')  
     use iso_c_binding
     implicit none
        character(kind=c_char), intent(in) :: name(*)
        integer(kind=c_int)                :: ierr
     end function c_vt_timer_tic

     function c_vt_timer_toc(name) result(ierr) bind(c,name='vt_timer_toc')
     use iso_c_binding
     implicit none
        character(kind=c_char), intent(in) :: name(*)
        integer(kind=c_int)                :: ierr
     end function c_vt_timer_toc

     function c_vt_timers_to_cstring(cstring, n) result(ierr)bind(c,name='vt_timer_toc') 
     use iso_c_binding
     implicit none
        character(kind=c_char), intent(in) :: cstring(*)
        integer(kind=c_size_t), intent(in), value  :: n
        integer(kind=c_int)                :: ierr
     end function c_vt_timers_to_cstring

end interface
contains
     subroutine vt_last_error_message(string)
     use iso_c_binding
     implicit none
        character(len=*), intent(out)  :: string
        call c_vt_last_error_message(string,int(len(string),c_size_t))
     end subroutine vt_last_error_message

     function vt_timer_tic(name) result(ierr)
     use iso_c_binding
     implicit none
        character(len=*), intent(in) :: name
        integer                      :: ierr
        ierr = c_vt_timer_tic(trim(name)//char(0))
     end function vt_timer_tic

     function vt_timer_toc(name) result(ierr)
     use iso_c_binding
     implicit none
        character(len=*), intent(in) :: name
        integer                      :: ierr
        ierr = c_vt_timer_toc(trim(name)//char(0))
     end function vt_timer_toc

     function vt_timers_to_string(string) result(ierr)
     use iso_c_binding
     implicit none
        character(len=*), intent(in) :: string
        integer                      :: ierr
        ierr =  c_vt_timers_to_cstring(string,int(len(string),kind=c_size_t))
     end function vt_timers_to_string
end module vt_timers

