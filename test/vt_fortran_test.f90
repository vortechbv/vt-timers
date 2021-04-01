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
module m_sleep
implicit none
    interface
       subroutine sleep(milliseconds) bind(c,name='sleep')
       use iso_c_binding
       implicit none
         real(c_double), intent(in), value :: milliseconds
       end subroutine sleep
    end interface
end module m_sleep

module m_test_correct_usage
implicit none
contains 
    function test_correct_usage() bind(c,name='f_test_correct_usage') result(ierr)
    use m_sleep
    use vt_timers
    use iso_c_binding
    implicit none
        integer(c_int) :: ierr

        integer :: i
        ierr = vtOK

        if (ierr == vtOK) ierr = vt_timer_tic("label1");
            call sleep(200d0);
        if (ierr == vtOK) ierr = vt_timer_toc("label1");

        if (ierr == vtOK) ierr = vt_timer_tic("label2");
            call sleep(100d0);
        if (ierr == vtOK) ierr = vt_timer_toc("label2");


        if (ierr == vtOK) ierr = vt_timer_tic("label3");

            call sleep(100d0);

            if (ierr == vtOK) ierr = vt_timer_tic("label4");

                call sleep(50d0);

                if (ierr == vtOK) ierr = vt_timer_tic("label 5");
                    call sleep(10d0);
                if (ierr == vtOK) ierr = vt_timer_toc("label 5");

                if (ierr == vtOK) ierr = vt_timer_tic("label 6");
                    call sleep(10d0);
                if (ierr == vtOK) ierr = vt_timer_toc("label 6");

            if (ierr == vtOK) ierr = vt_timer_toc("label4");

            if (ierr == vtOK) ierr = vt_timer_tic("label4");

                call sleep(50d0);

                if (ierr == vtOK) ierr = vt_timer_tic("label 5");
                    call sleep(10d0);
                if (ierr == vtOK) ierr = vt_timer_toc("label 5");

                if (ierr == vtOK) ierr = vt_timer_tic("label 6");
                    call sleep(10d0);
                if (ierr == vtOK) ierr = vt_timer_toc("label 6");

            if (ierr == vtOK) ierr = vt_timer_toc("label4");

        if (ierr == vtOK) ierr = vt_timer_toc("label3");

        if (ierr == vtOK) ierr = vt_timer_tic("for loop");
            do i = 1,100
                if (ierr == vtOK) ierr = vt_timer_tic("inner iteration");
                call sleep(10d0);
                if (ierr == vtOK) ierr = vt_timer_toc("inner iteration");
            end do
        if (ierr == vtOK) ierr = vt_timer_toc("for loop");

        if (ierr == vtOK) ierr = vt_timers_to_stdout();

    if (ierr == vtOK) ierr = vt_timers_reset();

    end function test_correct_usage

end module m_test_correct_usage

module m_correct_usage_omp
implicit none
contains
    function correct_usage_omp() bind(c,name='f_correct_usage_omp') result(ierr)
    use omp_lib
    use m_sleep
    use vt_timers
    use iso_c_binding
    implicit none
       integer(c_int) :: ierr
       integer :: i
       integer, parameter :: n_iterations = 100;
       logical, parameter :: verbose = .false.

          ierr = vtOK

          if (ierr == vtOK) ierr = vt_timer_tic("top level");
         
         
!$omp     parallel do
          do i = 0, n_iterations-1
              if (verbose) then
                  print '(10(a,i0))', "nr omp threads: ", omp_get_num_threads(), &
                                      ", this is thread ", omp_get_thread_num()
              endif
         
              if (ierr == vtOK) ierr = vt_timer_tic("inside omp loop");
                 if (ierr == vtOK) ierr = vt_timer_tic("deeper inside omp loop");
                     call sleep(10d0 + 10d0 * omp_get_thread_num());
                 if (ierr == vtOK) ierr = vt_timer_toc("deeper inside omp loop");
              if (ierr == vtOK) ierr = vt_timer_toc("inside omp loop");
          end do
!$OMP     end parallel do
         
          if (ierr == vtOK) ierr = vt_timer_tic("some more work");
          if (ierr == vtOK) ierr = vt_timer_toc("some more work");
         
          if (ierr == vtOK) ierr = vt_timer_toc("top level");
         
          if (ierr == vtOK) ierr = vt_timers_to_stdout();
         
          if (ierr == vtOK) ierr = vt_timers_reset();
   end function correct_usage_omp
end module m_correct_usage_omp 

