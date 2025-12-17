CREATE OR REPLACE VIEW student_avg_grades AS
SELECT 
    s.student_id,
    s.full_name,
    s.group_number,
    ROUND(AVG(g.grade)::numeric, 2) as average_grade,
    COUNT(g.grade_id) as total_grades
FROM students s
LEFT JOIN grades g ON s.student_id = g.student_id
GROUP BY s.student_id, s.full_name, s.group_number
ORDER BY average_grade DESC;

CREATE OR REPLACE VIEW detailed_grades AS
SELECT 
    g.grade_id,
    s.full_name,
    s.group_number,
    sub.subject_name,
    g.grade,
    g.created_at
FROM grades g
JOIN students s ON g.student_id = s.student_id
JOIN subjects sub ON g.subject_id = sub.subject_id
ORDER BY g.created_at DESC;

CREATE OR REPLACE VIEW attendance_stats AS
SELECT 
    s.student_id,
    s.full_name,
    s.group_number,
    COUNT(*) as total_classes,
    COUNT(CASE WHEN a.status = 'present' THEN 1 END) as present_count,
    COUNT(CASE WHEN a.status = 'absent' THEN 1 END) as absent_count,
    COUNT(CASE WHEN a.status = 'late' THEN 1 END) as late_count,
    ROUND((COUNT(CASE WHEN a.status = 'present' THEN 1 END) * 100.0 / COUNT(*))::numeric, 2) as attendance_percentage
FROM students s
LEFT JOIN attendance a ON s.student_id = a.student_id
GROUP BY s.student_id, s.full_name, s.group_number
ORDER BY attendance_percentage DESC;